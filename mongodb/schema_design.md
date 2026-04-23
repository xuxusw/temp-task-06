# MongoDB schema design для системы управления проектами (вариант 8)

## Выбор коллекций

в MongoDB созданы три коллекции для основных сущностей (`users`, `projects`, `tasks`) и три коллекции для хранения дополнительных данных, не подходящих для реляционной модели PostgreSQL.

ответы `replies` вложены в документ комментария `comments` (embedded), т.к. ответы не существуют без комментария и запрос комментария всегда подразумевает получение всех ответов. в рез-те нет необходимости искать ответы отдельно.

## Структура коллекций
1. `users` (дублирование из PostgreSQL):
```
{
  "_id": ObjectId,
  "login": String,              // уникальный логин
  "password_hash": String,      // хеш пароля
  "first_name": String,         // имя
  "last_name": String,          // фамилия
  "email": String,              // email
  "created_at": ISODate,        // дата регистрации
  "is_deleted": Boolean         // soft delete (stays in DB but not an active user)
}
```
###  Индексы
для поиска по логину:
```
db.users.createIndex({ login: 1 }, { unique: true });
```
для поиска по маске имени и фамилии:
```
db.users.createIndex({ first_name: 1, last_name: 1 });
```

2. `projects` (дублирование из PostgreSQL):
```
{
  "_id": ObjectId,
  "name": String,               // название проекта
  "description": String,        // описание
  "key": String,                // короткий код (PRJ1)
  "owner_id": Number,           // ID владельца (из PostgreSQL)
  "created_at": ISODate         // дата создания
}
```
###  Индексы
для поиска по названию:
```
db.projects.createIndex({ name: 1 });
```
для поиска по ID владельца:
```
db.projects.createIndex({ owner_id: 1 });
```

3. `tasks` (дублирование из PostgreSQL):
```
{
  "_id": ObjectId,
  "title": String,              // заголовок
  "description": String,        // описание
  "status": String,             // TODO, IN_PROGRESS, REVIEW, DONE
  "project_id": Number,         // ID проекта (из PostgreSQL)
  "assignee_id": Number,        // ID исполнителя (из PostgreSQL)
  "creator_id": Number,         // ID создателя (из PostgreSQL)
  "priority": Number,           // приоритет 1-5
  "created_at": ISODate,
  "updated_at": ISODate
}
```
###  Индексы
для поиска по ID проекта:
```
db.tasks.createIndex({ project_id: 1 });
```
для поиска по ID исполнителя:
```
db.tasks.createIndex({ assignee_id: 1 });
```
для поиска по ID создателя:
```
db.tasks.createIndex({ creator_id: 1 });
```
для поиска по приоритету:
```
db.tasks.createIndex({ status: 1 });
```

4. `comments` - комментарии к задачам:
```
{
  "_id": ObjectId,             // mongodb adds automaticly
  "task_id": Number,           // ссылка на задачу в PostgreSQL (reference)
  "author": String,            // имя автора
  "author_id": Number,         // ID автора из PostgreSQL (reference)
  "text": String,              // текст комментария
  "created_at": ISODate,       // дата создания
  "updated_at": ISODate,       // дата обновления (опционально)
  "replies": [                 // вложенные ответы (embedded)
    {
      "author": String,
      "text": String,
      "created_at": ISODate
    }
  ]
}
```
###  Индексы
для быстрого поиска по задаче: 
```
db.comments.createIndex({ task_id: 1 });
```
для сортировки по дате:
```
db.comments.createIndex({ created_at: -1 });
```

5. `notifications` - уведомления пользователей:
```
{
  "_id": ObjectId,
  "user_id": Number,           // ID пользователя из PostgreSQL
  "type": String,              // тип: comment, task_assigned, task_completed, project_created
  "message": String,           // текст уведомления
  "created_at": ISODate,       // дата создания
  "is_read": Boolean,          // прочитано ли
  "read_at": ISODate,          // дата прочтения (опционально)
  "metadata": {                // дополнительные данные (опционально)
    "task_id": Number,
    "project_id": Number,
    "comment_id": ObjectId
  }
}
```
###  Индексы
для получения уведомлений пользователя по дате: 
```
db.notifications.createIndex({ user_id: 1, created_at: -1 });
```
для фильтрации непрочитанных:
```
db.notifications.createIndex({ user_id: 1, is_read: 1 });
```

6. `task_history` - история изменений задач:
```
{
  "_id": ObjectId,
  "task_id": Number,           // ID задачи из PostgreSQL
  "changed_by": Number,        // ID пользователя, сделавшего изменение
  "field": String,             // имя измененного поля
  "old_value": String,         // старое значение
  "new_value": String,         // новое значение
  "changed_at": ISODate,       // время изменения
  "comment": String            // комментарий к изменению (опционально)
}
```
###  Индексы
для получения истории задачи по дате: 
```
db.task_history.createIndex({ task_id: 1, changed_at: -1 });
```
для поиска изменений, сделанных пользователем:
```
db.task_history.createIndex({ changed_by: 1 });
```

## Выбор между embedded и references
| Связь | Тип | Обоснование |
|-------|-------|---------|
| Пользователи | Reference | Дублируются из PostgreSQL |
| Задачи | Reference | Дублируются из PostgreSQL |
| Проекты | Reference | Дублируются из PostgreSQL |
| Комментарий -> Задача (task_id) | Reference | Задача хранится в PostgreSQL, связь по ID |
| Комментарий -> Автор (author_id) | Reference | Автор хранится в PostgreSQL, связь по ID |
| Комментарий -> Ответы | Embedded | Ответы не существуют без комментария, всегда запрашиваются вместе |
| Уведомление -> Пользователь | Reference (user_id) | Пользователь хранится в PostgreSQL |
| История задач -> Задача | Reference (task_id) | Задача хранится в PostgreSQL |

Все коллекции имеют валидацию через $jsonSchema.