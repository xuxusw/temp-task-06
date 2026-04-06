\c myservice_db;

-- создание нового пользователя
INSERT INTO users (login, password_hash, first_name, last_name, email)
VALUES ('newuser', 'pass123', 'Имя', 'Фамилия', 'email@example.com')
RETURNING id, login, first_name, last_name, email;


-- поиск пользователя по логину
SELECT id, login, first_name, last_name, email, created_at
FROM users
WHERE login = 'ivan123' AND is_deleted = FALSE;


-- поиск пользователя по маске имени и фамилии
SELECT id, login, first_name, last_name, email, created_at
FROM users
WHERE first_name ILIKE '%Иван%' AND last_name ILIKE '%Петров%' AND is_deleted = FALSE;


-- создание проекта
INSERT INTO projects (name, description, key, owner_id)
VALUES ('Новый проект', 'Описание', 'NEW', 1)
RETURNING id, name, description, key, owner_id, created_at;


-- поиск проекта по имени
SELECT id, name, description, key, owner_id, created_at
FROM projects
WHERE name ILIKE '%Альфа%';


-- получение всех проектов
SELECT id, name, description, key, owner_id, created_at
FROM projects
ORDER BY created_at DESC;


-- создание задачи в проекте
INSERT INTO tasks (title, description, project_id, creator_id, assignee_id, priority)
VALUES ('Новая задача', 'Описание задачи', 1, 1, 2, 4)
RETURNING id, title, description, status, project_id, creator_id, assignee_id, priority, created_at;


-- получение всех задач в проекте
SELECT t.id, t.title, t.description, t.status, t.priority,
       t.created_at, t.updated_at,
       u.first_name || ' ' || u.last_name AS assignee_name
FROM tasks t
LEFT JOIN users u ON t.assignee_id = u.id AND u.is_deleted = FALSE
WHERE t.project_id = 1
ORDER BY t.priority DESC, t.created_at;


-- получение задачи по коду (ID)
SELECT t.id, t.title, t.description, t.status, t.priority,
       t.created_at, t.updated_at,
       p.name AS project_name,
       creator.first_name || ' ' || creator.last_name AS creator_name,
       assignee.first_name || ' ' || assignee.last_name AS assignee_name
FROM tasks t
JOIN projects p ON t.project_id = p.id
JOIN users creator ON t.creator_id = creator.id
LEFT JOIN users assignee ON t.assignee_id = assignee.id AND assignee.is_deleted = FALSE
WHERE t.id = 1;


-- обновление статуса задачи
UPDATE tasks
SET status = 'IN_PROGRESS', updated_at = CURRENT_TIMESTAMP
WHERE id = 1
RETURNING id, status, updated_at;


-- поиск задач по исполнителю
SELECT t.id, t.title, t.status, p.name AS project_name
FROM tasks t
JOIN projects p ON t.project_id = p.id
WHERE t.assignee_id = 2
ORDER BY t.created_at DESC;


-- статистика по проекту (количестсво задач по статусам)
SELECT 
    p.name AS project_name,
    COUNT(CASE WHEN t.status = 'TODO' THEN 1 END) AS todo_count,
    COUNT(CASE WHEN t.status = 'IN_PROGRESS' THEN 1 END) AS in_progress_count,
    COUNT(CASE WHEN t.status = 'REVIEW' THEN 1 END) AS review_count,
    COUNT(CASE WHEN t.status = 'DONE' THEN 1 END) AS done_count,
    COUNT(*) AS total_tasks
FROM projects p
LEFT JOIN tasks t ON p.id = t.project_id
WHERE p.id = 1
GROUP BY p.id, p.name;


-- user soft delete
UPDATE users SET is_deleted = TRUE WHERE id = 1;

-- get all users (even deleted)
SELECT id, login, first_name, last_name, email, is_deleted FROM users;

-- get only active users (is_deleted = F)
SELECT id, login, first_name, last_name, email
FROM users WHERE is_deleted = FALSE;

-- get the user back
UPDATE users SET is_deleted = FALSE WHERE id = 1;