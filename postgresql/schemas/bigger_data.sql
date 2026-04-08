\c myservice_db;

CREATE OR REPLACE FUNCTION random_string(length INTEGER) RETURNS TEXT AS $$
DECLARE
    chars TEXT := 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz';
    result TEXT := '';
BEGIN
    FOR i IN 1..length LOOP
        result := result || substr(chars, floor(random() * 52 + 1)::INTEGER, 1);
    END LOOP;
    RETURN result;
END;
$$ LANGUAGE plpgsql;

-- 10000 пользователей
INSERT INTO users (login, password_hash, first_name, last_name, email, is_deleted)
SELECT 
    'user_' || i,
    'hash_' || i,
    'Имя_' || i,
    'Фамилия_' || i,
    'user_' || i || '@example.com',
    FALSE
FROM generate_series(11, 10010) i;

-- 1000 проектов
INSERT INTO projects (name, description, key, owner_id)
SELECT 
    'Проект_' || i,
    'Описание проекта_' || i,
    'PRJ_' || i,
    (i % 10) + 1
FROM generate_series(11, 1010) i;

-- 5000 задач
INSERT INTO tasks (title, description, status, project_id, assignee_id, creator_id, priority)
SELECT 
    'Задача_' || i,
    'Описание задачи_' || i,
    CASE (i % 4)
        WHEN 0 THEN 'TODO'
        WHEN 1 THEN 'IN_PROGRESS'
        WHEN 2 THEN 'REVIEW'
        ELSE 'DONE'
    END,
    (i % 500) + 1,
    (i % 10) + 1,
    (i % 10) + 1,
    (i % 5) + 1
FROM generate_series(11, 5010) i;