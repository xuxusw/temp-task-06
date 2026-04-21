# MongoDB schema design для системы управления проектами (вариант 8)

## Выбор коллекций

в MongoDB созданы три коллекции для хранения дополнительных данных, не подходящих для реляционной модели PostgreSQL.

ответы `replies` вложены в документ комментария `comments` (embedded), т.к. ответы не существуют без комментария и запрос комментария всегда подразумевает получение всех ответов. в рез-те нет необходимости искать ответы отдельно.

## Структура коллекций
1. `comments` - комментарии к задачам:
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

2. `notifications` - уведомления пользователей:
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

3. `task_history` - история изменений задач:
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
| Комментарий -> Задача (task_id) | Reference | Задача хранится в PostgreSQL, связь по ID |
| Комментарий -> Автор (author_id) | Reference | Автор хранится в PostgreSQL, связь по ID |
| Комментарий -> Ответы | Embedded | Ответы не существуют без комментария, всегда запрашиваются вместе |
| Уведомление -> Пользователь | Reference (user_id) | Пользователь хранится в PostgreSQL |
| История -> Задача | Reference (task_id) | Задача хранится в PostgreSQL |

Все коллекции имеют валидацию через $jsonSchema.