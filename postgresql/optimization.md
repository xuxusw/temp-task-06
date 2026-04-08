## Оптимизация запросов PostgreSQL

## Используемые индексы (schema.sql):
1. `idx_users_login` - для поиска по логину (только активные пользователи)
2. `idx_users_first_name_last_name` - для поиска по имени и фамилии
3. `idx_projects_name` - для поиска проектов по имени
4. `idx_projects_owner_id` - для поиска проектов по создателю/
владельцу
5. `idx_tasks_project_id` - для JOIN и фильтрации по проекту
6. `idx_tasks_assignee_id` - для поиска задач по исполнителю
7. `idx_tasks_creator_id` - для поиска задач по создателю
8. `idx_tasks_status` - для поиска по задач по статусу
8. `idx_tasks_project_priority` - для сортировки по приоритету

### Индексы для оптимизации:
GIN индекс для поиска по маске:
```
CREATE EXTENSION IF NOT EXISTS pg_trgm;
CREATE INDEX idx_users_name_trgm ON users USING GIN (first_name gin_trgm_ops, last_name gin_trgm_ops) WHERE is_deleted = FALSE;
```

## EXPLAIN ANALYZE 
### 1. Поиск пользователя по логину
```
EXPLAIN ANALYZE SELECT * FROM users WHERE login = 'ivan123' AND is_deleted = FALSE;
```
Seq scan QUERY PLAN: 
```
 Seq Scan on users  (cost=0.00..1.12 rows=1 width=1599) (actual time=0.012..0.014 rows=1
 loops=1)
   Filter: ((NOT is_deleted) AND ((login)::text = 'ivan123'::text))
   Rows Removed by Filter: 9
 Planning Time: 0.955 ms
 Execution Time: 0.031 ms
(5 rows)
```
Index scan QUERY PLAN:
```
 Index Scan using idx_users_login on users  (cost=0.14..8.15 rows=1 width=1599) (actual 
time=0.014..0.015 rows=1 loops=1)
   Index Cond: ((login)::text = 'ivan123'::text)
 Planning Time: 0.106 ms
 Execution Time: 0.030 ms
(4 rows)
```

### 2. Поиск по маске имени и фамилии
```
EXPLAIN ANALYZE SELECT * FROM users WHERE first_name ILIKE '%Иван%' AND is_deleted = FALSE;
```
QUERY PLAN w/o GIN:
```
 Seq Scan on users  (cost=0.00..10.62 rows=1 width=1599) (actual time=0.109..0.119 rows=
1 loops=1)
   Filter: ((NOT is_deleted) AND ((first_name)::text ~~* '%Иван%'::text))
   Rows Removed by Filter: 9
 Planning Time: 8.767 ms
 Execution Time: 0.152 ms
(5 rows)
```
QUERY PLAN with GIN:
```
 Bitmap Heap Scan on users  (cost=4.16..8.22 rows=1 width=1599) (actual time=0.019..0.02
7 rows=1 loops=1)
   Recheck Cond: (NOT is_deleted)
   Filter: ((first_name)::text ~~* '%Иван%'::text)
   Rows Removed by Filter: 9
   Heap Blocks: exact=1
   ->  Bitmap Index Scan on idx_users_first_name_last_name  (cost=0.00..4.16 rows=5 widt
h=0) (actual time=0.006..0.007 rows=10 loops=1)
 Planning Time: 0.135 ms
 Execution Time: 0.050 ms
(8 rows)
```

### 3. Получение задач с JOIN
```
EXPLAIN ANALYZE SELECT t.*, u.first_name FROM tasks t LEFT JOIN users u ON t.assignee_id = u.id WHERE t.project_id = 1;
```
Seq scan QUERY PLAN:
```
 Nested Loop Left Join  (cost=0.00..12.72 rows=1 width=860) (actual time=0.020..0.023 ro
ws=1 loops=1)
   Join Filter: (t.assignee_id = u.id)
   ->  Seq Scan on tasks t  (cost=0.00..11.50 rows=1 width=642) (actual time=0.015..0.01
6 rows=1 loops=1)
         Filter: (project_id = 1)
         Rows Removed by Filter: 9
   ->  Seq Scan on users u  (cost=0.00..1.10 rows=10 width=222) (actual time=0.003..0.00
3 rows=1 loops=1)
 Planning Time: 0.173 ms
 Execution Time: 0.182 ms
(8 rows)
```
Index scan QUERY PLAN:
```
 Nested Loop Left Join  (cost=0.28..17.77 rows=1 width=860) (actual time=0.020..0.022 ro
ws=1 loops=1)
   ->  Index Scan using idx_tasks_project_priority on tasks t  (cost=0.14..8.16 rows=1 w
idth=642) (actual time=0.013..0.014 rows=1 loops=1)
         Index Cond: (project_id = 1)
   ->  Index Scan using users_pkey on users u  (cost=0.14..8.15 rows=1 width=222) (actua
l time=0.004..0.004 rows=1 loops=1)
         Index Cond: (id = t.assignee_id)
 Planning Time: 0.161 ms
 Execution Time: 0.045 ms
(7 rows)
```
Bitmap Heap Scan QUERY PLAN:
```
 Nested Loop Left Join  (cost=8.29..17.05 rows=1 width=860) (actual time=0.021..0.023 ro
ws=1 loops=1)
   ->  Bitmap Heap Scan on tasks t  (cost=4.15..8.16 rows=1 width=642) (actual time=0.01
3..0.014 rows=1 loops=1)
         Recheck Cond: (project_id = 1)
         Heap Blocks: exact=1
         ->  Bitmap Index Scan on idx_tasks_project_priority  (cost=0.00..4.15 rows=1 wi
dth=0) (actual time=0.009..0.009 rows=1 loops=1)
               Index Cond: (project_id = 1)
   ->  Bitmap Heap Scan on users u  (cost=4.14..8.16 rows=1 width=222) (actual time=0.00
4..0.005 rows=1 loops=1)
         Recheck Cond: (t.assignee_id = id)
         Heap Blocks: exact=1
         ->  Bitmap Index Scan on users_pkey  (cost=0.00..4.14 rows=1 width=0) (actual t
ime=0.002..0.002 rows=1 loops=1)
               Index Cond: (id = t.assignee_id)
 Planning Time: 0.167 ms
 Execution Time: 0.052 ms
(13 rows)
```

## Сравнение до и после оптимизации
| Query | W/o index | With index | Query acceleration |
|-------|------|-------------|-------------|
| search by login | 0.031 ms (seq scan) | 0.030 ms (index scan) | ~0x |
| search by mask | 0.152 ms (seq scan) | 0.050 ms (bitmap heap scan) | ~3x |
| JOIN tasks->users | 0.182 ms (seq scan) | 0.045 ms (index scan) | ~4x |

Для поиска по маске рекомендуется GIN индекс с pg_trgm (особенно на больших данных).