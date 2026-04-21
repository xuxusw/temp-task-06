db = db.getSiblingDB('myservice_mongo');


db.createCollection('comments', {
    validator: {
        $jsonSchema: {
            bsonType: 'object',
            required: ['task_id', 'author', 'author_id', 'text', 'created_at'],
            properties: {
                task_id: { bsonType: 'int' },
                author: { bsonType: 'string' },
                author_id: { bsonType: 'int' },
                text: { bsonType: 'string', minLength: 1, maxLength: 5000 },
                created_at: { bsonType: 'date' },
                updated_at: { bsonType: 'date' },
                replies: {
                    bsonType: 'array',
                    items: {
                        bsonType: 'object',
                        required: ['author', 'text', 'created_at'],
                        properties: {
                            author: { bsonType: 'string' },
                            text: { bsonType: 'string', minLength: 1 },
                            created_at: { bsonType: 'date' }
                        }
                    }
                }
            }
        }
    }
});

db.comments.createIndex({ task_id: 1 });
db.comments.createIndex({ created_at: -1 });


db.createCollection('notifications', {
    validator: {
        $jsonSchema: {
            bsonType: 'object',
            required: ['user_id', 'type', 'message', 'created_at', 'is_read'],
            properties: {
                user_id: { bsonType: 'int' },
                type: { 
                    bsonType: 'string',
                    enum: ['comment', 'task_assigned', 'task_completed', 'project_created']
                },
                message: { bsonType: 'string' },
                created_at: { bsonType: 'date' },
                is_read: { bsonType: 'bool' },
                read_at: { bsonType: 'date' },
                metadata: {
                    bsonType: 'object',
                    properties: {
                        task_id: { bsonType: 'int' },
                        project_id: { bsonType: 'int' },
                        comment_id: { bsonType: 'objectId' }
                    }
                }
            }
        }
    }
});

db.notifications.createIndex({ user_id: 1, created_at: -1 });
db.notifications.createIndex({ user_id: 1, is_read: 1 });


db.createCollection('task_history', {
    validator: {
        $jsonSchema: {
            bsonType: 'object',
            required: ['task_id', 'changed_by', 'field', 'old_value', 'new_value', 'changed_at'],
            properties: {
                task_id: { bsonType: 'int' },
                changed_by: { bsonType: 'int' },
                field: { bsonType: 'string' },
                old_value: { bsonType: 'string' },
                new_value: { bsonType: 'string' },
                changed_at: { bsonType: 'date' },
                comment: { bsonType: 'string' }
            }
        }
    }
});

db.task_history.createIndex({ task_id: 1, changed_at: -1 });
db.task_history.createIndex({ changed_by: 1 });
