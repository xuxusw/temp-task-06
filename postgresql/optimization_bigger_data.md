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
 Seq Scan on users  (cost=0.00..13.38 rows=1 width=63) (actual time=0.014..0.087 rows=1 
loops=1)
   Filter: ((NOT is_deleted) AND ((login)::text = 'ivan123'::text))
   Rows Removed by Filter: 509
   Buffers: shared hit=7
 Planning:
   Buffers: shared hit=1
 Planning Time: 0.541 ms
 Execution Time: 0.101 ms
(8 rows)
```
Index scan QUERY PLAN:
```
 Index Scan using idx_users_login on users  (cost=0.27..8.29 rows=1 width=63) (actual ti
me=0.018..0.019 rows=1 loops=1)
   Index Cond: ((login)::text = 'ivan123'::text)
   Buffers: shared hit=3
 Planning:
   Buffers: shared hit=1
 Planning Time: 0.112 ms
 Execution Time: 0.033 ms
(7 rows)
```

### 2. Поиск по маске имени и фамилии
```
EXPLAIN ANALYZE SELECT * FROM users WHERE first_name = 'Иван' AND is_deleted = FALSE;
```
QUERY PLAN w/o GIN:
```
 Seq Scan on users  (cost=0.00..13.38 rows=51 width=63) (actual time=0.017..0.280 rows=5
1 loops=1)
   Filter: ((NOT is_deleted) AND ((first_name)::text = 'Иван'::text))
   Rows Removed by Filter: 459
 Planning Time: 0.116 ms
 Execution Time: 0.426 ms
(5 rows)
```
QUERY PLAN with GIN:
```
 Bitmap Heap Scan on users  (cost=4.67..12.31 rows=51 width=63) (actual time=0.025..0.04
0 rows=51 loops=1)
   Recheck Cond: (((first_name)::text = 'Иван'::text) AND (NOT is_deleted))
   Heap Blocks: exact=7
   ->  Bitmap Index Scan on idx_users_first_name_last_name  (cost=0.00..4.66 rows=51 wid
th=0) (actual time=0.018..0.018 rows=51 loops=1)
         Index Cond: ((first_name)::text = 'Иван'::text)
 Planning Time: 0.207 ms
 Execution Time: 0.070 ms
(7 rows)
```

### 3. Получение задач с JOIN
```
EXPLAIN ANALYZE SELECT t.*, u.first_name FROM tasks t LEFT JOIN users u ON t.assignee_id = u.id WHERE t.project_id = 1;
```
Seq scan QUERY PLAN:
```
 Hash Left Join  (cost=18.48..216.14 rows=15 width=100) (actual time=0.188..2.222 rows=1
5 loops=1)
   Hash Cond: (t.assignee_id = u.id)
   ->  Seq Scan on tasks t  (cost=0.00..197.62 rows=15 width=94) (actual time=0.011..2.0
28 rows=15 loops=1)
         Filter: (project_id = 1)
         Rows Removed by Filter: 6995
   ->  Hash  (cost=12.10..12.10 rows=510 width=10) (actual time=0.169..0.170 rows=510 lo
ops=1)
         Buckets: 1024  Batches: 1  Memory Usage: 31kB
         ->  Seq Scan on users u  (cost=0.00..12.10 rows=510 width=10) (actual time=0.00
5..0.092 rows=510 loops=1)
 Planning Time: 1.203 ms
 Execution Time: 2.501 ms
(10 rows)
```
Index scan QUERY PLAN:
```
 Merge Left Join  (cost=64.85..65.69 rows=15 width=100) (actual time=0.042..0.048 rows=1
5 loops=1)
   Merge Cond: (t.assignee_id = u.id)
   ->  Sort  (cost=64.58..64.62 rows=15 width=94) (actual time=0.031..0.033 rows=15 loop
s=1)
         Sort Key: t.assignee_id
         Sort Method: quicksort  Memory: 27kB
         ->  Index Scan using idx_tasks_project_id on tasks t  (cost=0.28..64.29 rows=15
 width=94) (actual time=0.009..0.025 rows=15 loops=1)
               Index Cond: (project_id = 1)
   ->  Index Scan using users_pkey on users u  (cost=0.27..33.92 rows=510 width=10) (act
ual time=0.005..0.005 rows=1 loops=1)
 Planning Time: 1.771 ms
 Execution Time: 0.075 ms
(10 rows)
```
Bitmap Heap Scan QUERY PLAN:
```
 Nested Loop Left Join  (cost=5.76..93.72 rows=15 width=100) (actual time=0.026..0.055 r
ows=15 loops=1)
   ->  Bitmap Heap Scan on tasks t  (cost=4.40..47.97 rows=15 width=94) (actual time=0.0
15..0.034 rows=15 loops=1)
         Recheck Cond: (project_id = 1)
         Heap Blocks: exact=15
         ->  Bitmap Index Scan on idx_tasks_project_priority  (cost=0.00..4.39 rows=15 w
idth=0) (actual time=0.009..0.009 rows=15 loops=1)
               Index Cond: (project_id = 1)
   ->  Memoize  (cost=1.36..5.37 rows=1 width=10) (actual time=0.001..0.001 rows=1 loops
=15)
         Cache Key: t.assignee_id
         Cache Mode: logical
         Hits: 14  Misses: 1  Evictions: 0  Overflows: 0  Memory Usage: 1kB
         ->  Bitmap Heap Scan on users u  (cost=1.35..5.36 rows=1 width=10) (actual time
=0.004..0.004 rows=1 loops=1)
               Recheck Cond: (t.assignee_id = id)
               Heap Blocks: exact=1
               ->  Bitmap Index Scan on users_pkey  (cost=0.00..1.35 rows=1 width=0) (ac
tual time=0.002..0.002 rows=1 loops=1)
                     Index Cond: (id = t.assignee_id)
 Planning Time: 0.650 ms
 Execution Time: 0.092 ms
(17 rows)
```

## Сравнение до и после оптимизации
| Query | W/o index | With index | Query acceleration |
|-------|------|-------------|-------------|
| search by login | 0.101 ms (seq scan) | 0.033 ms (index scan) | ~3x |
| search by mask | 0.426 ms (seq scan) | 0.070 ms (bitmap heap scan) | ~6x |
| JOIN tasks->users | 2.501 ms (seq scan) | 0.075 ms (index scan) | ~33x |

Для поиска по маске рекомендуется GIN индекс с pg_trgm (особенно на больших данных).  
После добавления индексов планировщик переключился с Hash Join на Merge Join и Nested Loop Join, что дало значительный прирост производительности.