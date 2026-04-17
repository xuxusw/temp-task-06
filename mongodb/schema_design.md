# MongoDB schema design для системы управления проектами (вариант 8)

## Выбор коллекций

в MongoDB реализована коллекция `comments` для хранения комментариев к задачам.

ответы `replies` вложены в документ комментария `comments` (embedded), т.к. ответы не существуют без комментария и запрос комментария всегда подразумевает получение всех ответов. в рез-те нет необходимости искать ответы отдельно.

## Структура документа
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

## Выбор между embedded и references
| Связь | Тип | Обоснование |
|-------|-------|---------|
| Комментарий -> Задача (task_id) | Reference | Задача хранится в PostgreSQL, связь по ID |
| Комментарий -> Автор (author_id) | Reference | Автор хранится в PostgreSQL, связь по ID |
| Комментарий -> Ответы (replies) | Embedded | Ответы не существуют без комментария, всегда запрашиваются вместе |

## Индексы
для быстрого поиска по задаче: 
```
db.comments.createIndex({ task_id: 1 });
```
для сортировки по дате:
```
db.comments.createIndex({ created_at: -1 });
```