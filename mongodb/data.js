db = db.getSiblingDB('myservice_mongo');

db.users.drop();
db.projects.drop();
db.tasks.drop();
db.comments.drop();
db.notifications.drop();
db.task_history.drop();


const users = [
    { login: 'ivan123', password_hash: 'hash_ivan', first_name: 'Иван', last_name: 'Петров', email: 'ivan@example.com', created_at: new Date(), is_deleted: false },
    { login: 'john', password_hash: 'hash_john', first_name: 'John', last_name: 'Doe', email: 'john@example.com', created_at: new Date(), is_deleted: false },
    { login: 'jane', password_hash: 'hash_jane', first_name: 'Jane', last_name: 'Smith', email: 'jane@example.com', created_at: new Date(), is_deleted: false },
    { login: 'alex', password_hash: 'hash_alex', first_name: 'Alex', last_name: 'Johnson', email: 'alex@example.com', created_at: new Date(), is_deleted: false },
    { login: 'maria', password_hash: 'hash_maria', first_name: 'Maria', last_name: 'Garcia', email: 'maria@example.com', created_at: new Date(), is_deleted: false },
    { login: 'dmitry', password_hash: 'hash_dmitry', first_name: 'Дмитрий', last_name: 'Соколов', email: 'dmitry@example.com', created_at: new Date(), is_deleted: false },
    { login: 'elena', password_hash: 'hash_elena', first_name: 'Елена', last_name: 'Кузнецова', email: 'elena@example.com', created_at: new Date(), is_deleted: false },
    { login: 'pavel', password_hash: 'hash_pavel', first_name: 'Павел', last_name: 'Морозов', email: 'pavel@example.com', created_at: new Date(), is_deleted: false },
    { login: 'olga', password_hash: 'hash_olga', first_name: 'Ольга', last_name: 'Волкова', email: 'olga@example.com', created_at: new Date(), is_deleted: false },
    { login: 'sergey', password_hash: 'hash_sergey', first_name: 'Сергей', last_name: 'Михайлов', email: 'sergey@example.com', created_at: new Date(), is_deleted: false }
];
db.users.insertMany(users);
print(`Inserted ${db.users.count()} users`);


const projects = [
    { name: 'Проект Альфа', description: 'Описание проекта Альфа', key: 'ALPHA', owner_id: 1, created_at: new Date() },
    { name: 'Проект Бета', description: 'Описание проекта Бета', key: 'BETA', owner_id: 1, created_at: new Date() },
    { name: 'Проект Гамма', description: 'Описание проекта Гамма', key: 'GAMMA', owner_id: 2, created_at: new Date() },
    { name: 'Frontend Redesign', description: 'Переделка фронтенда', key: 'FRONT', owner_id: 3, created_at: new Date() },
    { name: 'Backend API', description: 'Разработка API', key: 'BACK', owner_id: 1, created_at: new Date() },
    { name: 'Mobile App', description: 'Мобильное приложение', key: 'MOBILE', owner_id: 4, created_at: new Date() },
    { name: 'Database Migration', description: 'Миграция базы данных', key: 'DBMIG', owner_id: 2, created_at: new Date() },
    { name: 'DevOps', description: 'Настройка CI/CD', key: 'DEVOPS', owner_id: 5, created_at: new Date() },
    { name: 'Security Audit', description: 'Аудит безопасности', key: 'SEC', owner_id: 1, created_at: new Date() },
    { name: 'Documentation', description: 'Написание документации', key: 'DOCS', owner_id: 3, created_at: new Date() }
];
db.projects.insertMany(projects);
print(`Inserted ${db.projects.count()} projects`);


const tasks = [
    { title: 'Создать схему БД', description: 'Проектирование базы данных', status: 'DONE', project_id: 1, assignee_id: 1, creator_id: 1, priority: 5, created_at: new Date() },
    { title: 'Написать API регистрации', description: 'Регистрация пользователей', status: 'DONE', project_id: 5, assignee_id: 2, creator_id: 1, priority: 5, created_at: new Date() },
    { title: 'Написать API логина', description: 'Аутентификация', status: 'IN_PROGRESS', project_id: 5, assignee_id: 2, creator_id: 1, priority: 4, created_at: new Date() },
    { title: 'Разработать фронтенд', description: 'Главная страница', status: 'TODO', project_id: 4, assignee_id: 3, creator_id: 3, priority: 3, created_at: new Date() },
    { title: 'Настроить CI/CD', description: 'GitHub Actions', status: 'IN_PROGRESS', project_id: 8, assignee_id: 5, creator_id: 5, priority: 4, created_at: new Date() },
    { title: 'Написать тесты', description: 'Модульное тестирование', status: 'TODO', project_id: 5, assignee_id: 2, creator_id: 1, priority: 3, created_at: new Date() },
    { title: 'Документация API', description: 'OpenAPI спецификация', status: 'REVIEW', project_id: 10, assignee_id: 3, creator_id: 3, priority: 2, created_at: new Date() },
    { title: 'Аудит безопасности', description: 'Проверка уязвимостей', status: 'TODO', project_id: 9, assignee_id: 1, creator_id: 1, priority: 5, created_at: new Date() },
    { title: 'Оптимизация запросов', description: 'Добавить индексы', status: 'IN_PROGRESS', project_id: 7, assignee_id: 2, creator_id: 2, priority: 4, created_at: new Date() },
    { title: 'Деплой в продакшн', description: 'Настройка сервера', status: 'TODO', project_id: 8, assignee_id: 5, creator_id: 5, priority: 5, created_at: new Date() }
];
db.tasks.insertMany(tasks);
print(`Inserted ${db.tasks.count()} tasks`);


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