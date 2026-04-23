// CRUD операции

db = db.getSiblingDB('myservice_mongo');

// users CRUD
db.users.insertOne({
    login: 'newuser',
    password_hash: 'hash_new',
    first_name: 'New',
    last_name: 'User',
    email: 'new@example.com',
    created_at: new Date(),
    is_deleted: false
});

db.users.findOne({ login: 'ivan123', is_deleted: false });
db.users.find({ first_name: /Иван/i, is_deleted: false });
db.users.find({ is_deleted: false });

db.users.updateOne({ login: 'newuser' }, { $set: { first_name: 'Updated' } });

db.users.deleteOne({ login: 'newuser' });

// projects CRUD
db.projects.insertOne({
    name: 'CRUD Test Project',
    description: 'Test',
    key: 'CRUD',
    owner_id: 1,
    created_at: new Date()
});

db.projects.findOne({ key: 'CRUD' });
db.projects.updateOne({ key: 'CRUD' }, { $set: { name: 'Updated CRUD Project' } });
db.projects.deleteOne({ key: 'CRUD' });

db.projects.find({ name: /Альфа/i });
db.projects.find().sort({ created_at: -1 });

// tasks CRUD
db.tasks.insertOne({
    title: 'CRUD Test Task',
    description: 'Test task',
    status: 'TODO',
    project_id: 1,
    creator_id: 1,
    priority: 3,
    created_at: new Date()
});
db.tasks.findOne({ title: 'CRUD Test Task' });
db.tasks.updateOne({ title: 'CRUD Test Task' }, { $set: { status: 'DONE' } });
db.tasks.deleteOne({ title: 'CRUD Test Task' });

db.tasks.find({ project_id: 1 }).sort({ priority: -1 });
db.tasks.find({ assignee_id: 1 });
db.tasks.updateOne(
    { _id: db.tasks.findOne()._id },
    { $set: { status: 'DONE', updated_at: new Date() } }
);


db.comments.insertOne({
    task_id: 10,
    author: 'Тестовый Пользователь',
    author_id: 11,
    text: 'новый комментарий',
    created_at: new Date(),
    replies: []
});


db.comments.find({ task_id: 1 }).pretty();
db.notifications.find({ user_id: 1 }).pretty();
db.task_history.find({ task_id: 1 }).pretty();

db.comments.find({ replies: { $ne: [] } }).pretty();

db.comments.find({
    created_at: {
        $gt: ISODate('2024-01-15T00:00:00Z'),
        $lt: ISODate('2024-01-20T00:00:00Z')
    }
}).pretty();

db.comments.find({ text: { $regex: 'документация', $options: 'i' } }).pretty();

db.comments.find({ author: { $in: ['Иван Петров', 'John Doe'] } }).pretty();


db.comments.updateOne(
    { _id: ObjectId("...") },
    { $set: { text: 'Обновленный текст', updated_at: new Date() } }
);

db.comments.updateOne(
    { _id: ObjectId("...") },
    { $push: { replies: { author: 'Новый автор', text: 'Текст ответа', created_at: new Date() } } }
);


db.notifications.insertOne({
    user_id: 1,
    type: 'comment',
    message: 'Кто-то ответил на ваш комментарий',
    created_at: new Date(),
    is_read: false,
    metadata: { task_id: 1, comment_id: newComment._id }
});
print('Added notification');


db.notifications.updateOne(
    { user_id: 1, is_read: false },
    { $set: { is_read: true, read_at: new Date() } }
);
print('Marked notifications as read');


db.comments.deleteOne({ _id: ObjectId("...") });

db.comments.deleteMany({ task_id: 999 });


// количество комментариев по задачам
db.comments.aggregate([
    { $group: { _id: '$task_id', count: { $sum: 1 } } },
    { $sort: { count: -1 } }
]).forEach(printjson);

// комментарии с количеством ответов
db.comments.aggregate([
    { $project: { author: 1, text: 1, replies_count: { $size: '$replies' } } },
    { $sort: { replies_count: -1 } }
]).forEach(printjson);


db.notifications.aggregate([
    { $group: { _id: '$user_id', count: { $sum: 1 } } },
    { $sort: { count: -1 } }
]).forEach(printjson);

// validation test - невалидный документ (должна быть ошибка)
try {
    db.comments.insertOne({
        task_id: 100,
        text: 'Нет обязательного поля author',
        created_at: new Date()
    });
    print('ERROR: Validation should have failed');
} catch(e) {
    print('Validation error (expected): ' + e);
}