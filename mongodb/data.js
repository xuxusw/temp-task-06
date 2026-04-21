db = db.getSiblingDB('myservice_mongo');

db.comments.drop();
db.notifications.drop();
db.task_history.drop();

db.comments.insertMany([
    {
        task_id: 1,
        author: 'Иван Петров',
        author_id: 1,
        text: 'Принял. Приступаю к выполнению.',
        created_at: new Date('2024-01-15T10:00:00Z'),
        replies: []
    },
    {
        task_id: 1,
        author: 'John Doe',
        author_id: 2,
        text: 'Нужно добавить документацию.',
        created_at: new Date('2024-01-15T11:30:00Z'),
        replies: [
            {
                author: 'Иван Петров',
                text: 'Согласен, добавлю',
                created_at: new Date('2024-01-15T12:00:00Z')
            }
        ]
    },
    {
        task_id: 2,
        author: 'Jane Smith',
        author_id: 3,
        text: 'Какие требования к производительности?',
        created_at: new Date('2024-01-16T09:00:00Z'),
        replies: []
    },
    {
        task_id: 2,
        author: 'Alex Johnson',
        author_id: 4,
        text: 'Нужно уточнить у заказчика',
        created_at: new Date('2024-01-16T10:15:00Z'),
        replies: [
            {
                author: 'Jane Smith',
                text: 'Уже уточняю, жду ответа',
                created_at: new Date('2024-01-16T11:00:00Z')
            }
        ]
    },
    {
        task_id: 3,
        author: 'Maria Garcia',
        author_id: 5,
        text: 'Сделано, проверьте',
        created_at: new Date('2024-01-17T14:00:00Z'),
        replies: []
    },
    {
        task_id: 3,
        author: 'Иван Петров',
        author_id: 1,
        text: 'Отлично, принимаю',
        created_at: new Date('2024-01-17T15:30:00Z'),
        replies: []
    },
    {
        task_id: 4,
        author: 'Дмитрий Соколов',
        author_id: 6,
        text: 'В работе',
        created_at: new Date('2024-01-18T08:00:00Z'),
        replies: []
    },
    {
        task_id: 5,
        author: 'Елена Кузнецова',
        author_id: 7,
        text: 'Есть вопросы по интеграции',
        created_at: new Date('2024-01-18T09:45:00Z'),
        replies: []
    },
    {
        task_id: 6,
        author: 'Павел Морозов',
        author_id: 8,
        text: 'Готово',
        created_at: new Date('2024-01-19T12:00:00Z'),
        replies: []
    },
    {
        task_id: 7,
        author: 'Ольга Волкова',
        author_id: 9,
        text: 'Нужна помощь',
        created_at: new Date('2024-01-19T16:20:00Z'),
        replies: []
    },
    {
        task_id: 8,
        author: 'Сергей Михайлов',
        author_id: 10,
        text: 'Сделал, проверьте пожалуйста',
        created_at: new Date('2024-01-20T11:00:00Z'),
        replies: [
            {
                author: 'Иван Петров',
                text: 'Хорошо, посмотрю сегодня',
                created_at: new Date('2024-01-20T11:30:00Z')
            }
        ]
    }
]);

print('Inserted ' + db.comments.count() + ' comments');


const notifications = [
    { user_id: 1, type: 'comment', message: 'Новый комментарий к задаче #1', created_at: new Date(), is_read: false },
    { user_id: 1, type: 'task_assigned', message: 'Вам назначена задача #5', created_at: new Date(), is_read: true, read_at: new Date() },
    { user_id: 2, type: 'task_completed', message: 'Задача #3 завершена', created_at: new Date(), is_read: false },
    { user_id: 3, type: 'project_created', message: 'Создан новый проект "Frontend Redesign"', created_at: new Date(), is_read: false },
    { user_id: 1, type: 'comment', message: 'Ответ на ваш комментарий', created_at: new Date(), is_read: false, metadata: { task_id: 1 } }
];

db.notifications.insertMany(notifications);
print(`Inserted ${db.notifications.count()} notifications`);


const taskHistory = [
    { task_id: 1, changed_by: 1, field: 'status', old_value: 'TODO', new_value: 'IN_PROGRESS', changed_at: new Date() },
    { task_id: 1, changed_by: 2, field: 'assignee_id', old_value: 'null', new_value: '5', changed_at: new Date() },
    { task_id: 2, changed_by: 1, field: 'priority', old_value: '3', new_value: '5', changed_at: new Date(), comment: 'Критическая задача' }
];

db.task_history.insertMany(taskHistory);
print(`Inserted ${db.task_history.count()} task_history records`);