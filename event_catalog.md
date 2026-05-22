## События и команды

| Команда  | Событие | Топик |
|-------------------|-----------------|-------|
| POST /api/register | UserRegistered | user-events |
| POST /api/login | UserLoggedIn | user-events |
| POST /api/projects | ProjectCreated | project-events |
| POST /api/projects/{id}/tasks | TaskCreated | task-events |
| POST /api/tasks/{id}/comments | CommentAdded | comment-events |

### Топики

| Топик | События | Ключ | Retention |
|-------|---------|------|-----------|
| user-events | UserRegistered, UserLoggedIn | user_id | 7 days |
| project-events | ProjectCreated | project_id | 7 days |
| task-events | TaskCreated | task_id | 7 days |
| comment-events | CommentAdded | comment_id | 7 days |

## Общая структура сообщения
```
{
  "event_id": "550e8400-e29b-41d4-a716-446655440000",
  "event_type": "EventName",
  "timestamp": "2026-05-20T10:30:00.000Z",
  "version": "1.0",
  "payload": { ... },
  "metadata": {
    "trace_id": "abc-123",
    "source_service": "myservice",
    "client_ip": "192.168.1.100"
  }
}
```

### 1. UserRegistered
Топик:	user-events  
Ключ:	user_id  
Производитель:	myservice (POST /api/register)  
Потребители:	event_consumer -> MongoDB (user_events)  
Гарантия доставки:	At-least-once  
Trigger:	Пользователь успешно зарегистрирован  
Payload Structure:  
```
{
  "user_id": 12345,
  "login": "john.doe",
  "email": "john@example.com",
  "first_name": "John",
  "last_name": "Doe"
}
```

### 2. UserLoggedIn
Топик:	user-events  
Ключ:	user_id  
Производитель:	myservice (POST /api/login)  
Потребители:	event_consumer -> MongoDB (user_events)  
Гарантия доставки:	At-least-once  
Trigger:	Пользователь успешно вошел в систем  
Payload Structure:  
```
{
  "user_id": 12345,
  "login": "john.doe",
  "ip_address": "192.168.1.100"
}
```

### 3. ProjectCreated
Топик:	project-events  
Ключ:	project_id  
Производитель:	myservice (POST /api/projects)  
Потребители:	event_consumer -> MongoDB (project_events)  
Гарантия доставки:	At-least-once  
Trigger:	Создан новый проект  
Payload Structure:  
```
{
  "project_id": 10,
  "name": "First Project",
  "key": "PRJ1",
  "owner_id": 12345
}
```

### 4. TaskCreated
Топик:	task-events  
Ключ:	task_id  
Производитель:	myservice (POST /api/projects/{id}/tasks)  
Потребители:	event_consumer -> MongoDB (task_events + task_analytics)  
Гарантия доставки:	At-least-once  
Trigger:	Создана новая задача в проекте  
Payload Structure:  
```
{
  "task_id": 42,
  "project_id": 10,
  "title": "Implement REST API",
  "description": "Create all endpoints",
  "priority": 5,
  "creator_id": 12345,
  "status": "TODO"
}
```

### 5. CommentAdded
Топик:	comment-events  
Ключ:	comment_id  
Производитель:	myservice (POST /api/tasks/{id}/comments)  
Потребители:	event_consumer -> MongoDB (comment_events)  
Гарантия доставки:	At-least-once  
Trigger:	Добавлен комментарий к задаче или ответ на комментарий  
Payload Structure:  
```
{
  "comment_id": "69ecc862cd332588e20220b5",
  "task_id": 42,
  "author": "John Doe",
  "text": "Great work!"
}
```