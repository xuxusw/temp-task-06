-- CREATE DATABASE myservice_db;

\c myservice_db;

CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    login VARCHAR(50) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    -- ill do NOT NULL cause we need to search via mask
    -- but in reality it still can be optional
    -- for corporate systems i guess its better to know first and last names of your colleagues
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    email VARCHAR(255) NOT NULL UNIQUE,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    is_deleted BOOLEAN DEFAULT FALSE -- reassigning, soft delete flag
);

-- WHERE is_deleted = FALSE 

CREATE TABLE IF NOT EXISTS projects (
    id SERIAL PRIMARY KEY,
    name VARCHAR(200) NOT NULL,
    description TEXT,
    key VARCHAR(10) UNIQUE, 
    owner_id INTEGER REFERENCES users(id) ON DELETE SET NULL, -- dont delete project after deleting the owner, just reassign later
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS tasks (
    id SERIAL PRIMARY KEY,
    title VARCHAR(500) NOT NULL,
    description TEXT,
    status VARCHAR(20) NOT NULL DEFAULT 'TODO',
    project_id INTEGER NOT NULL REFERENCES projects(id) ON DELETE CASCADE, -- deleting project deletes its tasks also
    assignee_id INTEGER REFERENCES users(id) ON DELETE SET NULL,
    creator_id INTEGER NOT NULL REFERENCES users(id) ON DELETE RESTRICT, -- keep the history?
    priority INTEGER DEFAULT 3,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    
    CONSTRAINT valid_status CHECK (status IN ('TODO', 'IN_PROGRESS', 'REVIEW', 'DONE')),
    CONSTRAINT valid_priority CHECK (priority BETWEEN 1 AND 5)
);


CREATE INDEX idx_users_login ON users(login) WHERE is_deleted = FALSE;
-- (WHERE first_name LIKE '%...%')
CREATE INDEX idx_users_first_name_last_name ON users(first_name, last_name) WHERE is_deleted = FALSE;

CREATE INDEX idx_projects_name ON projects(name);
CREATE INDEX idx_projects_owner_id ON projects(owner_id);

CREATE INDEX idx_tasks_project_id ON tasks(project_id);
CREATE INDEX idx_tasks_assignee_id ON tasks(assignee_id);
CREATE INDEX idx_tasks_creator_id ON tasks(creator_id);
CREATE INDEX idx_tasks_status ON tasks(status);
CREATE INDEX idx_tasks_project_priority ON tasks(project_id, priority DESC); -- sort by priority