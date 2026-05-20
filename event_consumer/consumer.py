#!/usr/bin/env python3
import json
import logging
import os
from kafka import KafkaConsumer
from pymongo import MongoClient
from datetime import datetime

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

class EventConsumer:
    def __init__(self):
        # Kafka config
        self.kafka_brokers = os.getenv('KAFKA_BROKERS', 'localhost:9092').split(',')
        self.consumer_group = os.getenv('CONSUMER_GROUP_ID', 'myservice-event-group')
        
        # MongoDB config
        mongo_uri = os.getenv('MONGO_URI', 'mongodb://localhost:27017')
        mongo_db = os.getenv('MONGO_DATABASE', 'myservice_mongo')
        
        self.mongo_client = MongoClient(mongo_uri)
        self.db = self.mongo_client[mongo_db]
        
        # collections for storing events
        self.user_events_col = self.db['user_events']
        self.project_events_col = self.db['project_events']
        self.task_events_col = self.db['task_events']
        self.comment_events_col = self.db['comment_events']
        
        # create indexes for deduplication
        self.user_events_col.create_index('event_id', unique=True)
        self.project_events_col.create_index('event_id', unique=True)
        self.task_events_col.create_index('event_id', unique=True)
        self.comment_events_col.create_index('event_id', unique=True)
        
        # create denormalized views (for CQRS)
        self.task_analytics_col = self.db['task_analytics']
        
        self.consumer = None
        
    def connect_kafka(self):
        """connect to Kafka with retries"""
        retries = 10
        for i in range(retries):
            try:
                self.consumer = KafkaConsumer(
                    'user-events', 'project-events', 'task-events', 'comment-events',
                    bootstrap_servers=self.kafka_brokers,
                    group_id=self.consumer_group,
                    auto_offset_reset='earliest',
                    enable_auto_commit=False,
                    value_deserializer=lambda m: json.loads(m.decode('utf-8')),
                    consumer_timeout_ms=1000
                )
                logger.info(f"Connected to Kafka brokers: {self.kafka_brokers}")
                return True
            except Exception as e:
                logger.warning(f"Kafka connection attempt {i+1}/{retries} failed: {e}")
                import time
                time.sleep(5)
        logger.error("Failed to connect to Kafka")
        return False
    
    def store_event(self, collection, event):
        """store event in MongoDB with deduplication"""
        try:
            collection.insert_one(event)
            return True
        except Exception as e:
            if 'duplicate key' in str(e):
                logger.warning(f"Duplicate event: {event.get('event_id')}")
                return True  # Already processed
            logger.error(f"Failed to store event: {e}")
            return False
    
    def update_task_analytics(self, event):
        """CQRS read model"""
        payload = event['payload']
        
        if event['event_type'] == 'TaskCreated':
            # Create analytics document
            self.task_analytics_col.update_one(
                {'task_id': payload['task_id']},
                {'$set': {
                    'task_id': payload['task_id'],
                    'project_id': payload['project_id'],
                    'title': payload['title'],
                    'priority': payload['priority'],
                    'status': 'TODO',
                    'created_at': event['timestamp'],
                    'last_updated': event['timestamp']
                }},
                upsert=True
            )
            logger.info(f"Updated task analytics for task {payload['task_id']}")
    
    def process_message(self, event):
        """process single event"""
        event_type = event.get('event_type')
        event_id = event.get('event_id')
        
        logger.info(f"Processing {event_type}: {event_id}")
        
        # Store in appropriate collection
        if event_type == 'UserRegistered' or event_type == 'UserLoggedIn':
            self.store_event(self.user_events_col, event)
        elif event_type == 'ProjectCreated':
            self.store_event(self.project_events_col, event)
        elif event_type == 'TaskCreated':
            self.store_event(self.task_events_col, event)
            self.update_task_analytics(event)
        elif event_type == 'CommentAdded':
            self.store_event(self.comment_events_col, event)
        else:
            logger.warning(f"Unknown event type: {event_type}")
        
        return True
    
    def run(self):
        """main loop"""
        if not self.connect_kafka():
            return
        
        logger.info("Event Consumer started. Waiting for messages...")
        
        try:
            for message in self.consumer:
                event = message.value
                
                try:
                    if self.process_message(event):
                        self.consumer.commit()
                        logger.info(f"Committed offset for event: {event.get('event_id')}")
                except Exception as e:
                    logger.error(f"Error processing message: {e}")
                    # don't commit, will retry
                    
        except KeyboardInterrupt:
            logger.info("Shutting down...")
        finally:
            self.consumer.close()
            self.mongo_client.close()

if __name__ == '__main__':
    consumer = EventConsumer()
    consumer.run()