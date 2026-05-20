## События и команды

| Команда (Command) | Событие (Event) | Топик |
|-------------------|-----------------|-------|
| POST /api/register | UserRegistered | user-events |
| POST /api/login | UserLoggedIn | user-events |
| POST /api/projects | ProjectCreated | project-events |
| POST /api/projects/{id}/tasks | TaskCreated | task-events |
| POST /api/tasks/{id}/comments | CommentAdded | comment-events |