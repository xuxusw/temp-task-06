\c myservice_db;

INSERT INTO users (login, password_hash, first_name, last_name, email, is_deleted) VALUES
('ivan123', 'hash_ivan', 'Иван', 'Петров', 'ivan@example.com', FALSE),
('john', 'hash_john', 'John', 'Doe', 'john@example.com', FALSE),
('jane', 'hash_jane', 'Jane', 'Smith', 'jane@example.com', FALSE),
('alex', 'hash_alex', 'Alex', 'Johnson', 'alex@example.com', FALSE),
('maria', 'hash_maria', 'Maria', 'Garcia', 'maria@example.com', FALSE),
('dmitry', 'hash_dmitry', 'Дмитрий', 'Соколов', 'dmitry@example.com', FALSE),
('elena', 'hash_elena', 'Елена', 'Кузнецова', 'elena@example.com', FALSE),
('pavel', 'hash_pavel', 'Павел', 'Морозов', 'pavel@example.com', FALSE),
('olga', 'hash_olga', 'Ольга', 'Волкова', 'olga@example.com', FALSE),
('sergey', 'hash_sergey', 'Сергей', 'Михайлов', 'sergey@example.com', FALSE);


INSERT INTO projects (name, description, key, owner_id) VALUES
('Проект Альфа', 'Описание проекта Альфа', 'ALPHA', 1),
('Проект Бета', 'Описание проекта Бета', 'BETA', 1),
('Проект Гамма', 'Описание проекта Гамма', 'GAMMA', 2),
('Frontend Redesign', 'Переделка фронтенда', 'FRONT', 3),
('Backend API', 'Разработка API', 'BACK', 1),
('Mobile App', 'Мобильное приложение', 'MOBILE', 4),
('Database Migration', 'Миграция базы данных', 'DBMIG', 2),
('DevOps', 'Настройка CI/CD', 'DEVOPS', 5),
('Security Audit', 'Аудит безопасности', 'SEC', 1),
('Documentation', 'Написание документации', 'DOCS', 3);


INSERT INTO tasks (title, description, status, project_id, assignee_id, creator_id, priority) VALUES
('Создать схему БД', 'Проектирование базы данных', 'DONE', 1, 1, 1, 5),
('Написать API регистрации', 'Регистрация пользователей', 'DONE', 5, 2, 1, 5),
('Написать API логина', 'Аутентификация', 'IN_PROGRESS', 5, 2, 1, 4),
('Разработать фронтенд', 'Главная страница', 'TODO', 4, 3, 3, 3),
('Настроить CI/CD', 'GitHub Actions', 'IN_PROGRESS', 8, 5, 5, 4),
('Написать тесты', 'Модульное тестирование', 'TODO', 5, 2, 1, 3),
('Документация API', 'OpenAPI спецификация', 'REVIEW', 10, 3, 3, 2),
('Аудит безопасности', 'Проверка уязвимостей', 'TODO', 9, 1, 1, 5),
('Оптимизация запросов', 'Добавить индексы', 'IN_PROGRESS', 7, 2, 2, 4),
('Деплой в продакшн', 'Настройка сервера', 'TODO', 8, 5, 5, 5);