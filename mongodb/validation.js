db = db.getSiblingDB('myservice_mongo');

db.createCollection('comments', {
    validator: {
        $jsonSchema: {
            bsonType: 'object',
            required: ['task_id', 'author', 'author_id', 'text', 'created_at'],
            properties: {
                task_id: {
                    bsonType: 'int',
                    description: 'ID задачи из PostgreSQL'
                },
                author: {
                    bsonType: 'string',
                    description: 'Имя автора'
                },
                author_id: {
                    bsonType: 'int',
                    description: 'ID автора из PostgreSQL'
                },
                text: {
                    bsonType: 'string',
                    minLength: 1,
                    maxLength: 5000,
                    description: 'Текст комментария'
                },
                created_at: {
                    bsonType: 'date',
                    description: 'Дата создания'
                },
                updated_at: {
                    bsonType: 'date',
                    description: 'Дата обновления'
                },
                replies: {
                    bsonType: 'array',
                    description: 'Вложенные ответы',
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