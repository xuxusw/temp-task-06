Start the container:
```
docker compose up -d mongodb
```
Run the command to see collections made: 
```
docker compose exec mongodb mongosh myservice_mongo --eval "show collections"
```
Enter the container to test queries:
```
docker compose exec mongodb mongosh myservice_mongo
```
Now you can run commands from `queries.js`.  
After you are done, stop and delete the container:
```
docker compose down -v
```

#### if scripts didnt exec automaticly:
Copy js scripts:
```
docker cp mongodb/validation.js myservice-mongodb-1:/tmp/validation.js
```

```
docker cp mongodb/data.js myservice-mongodb-1:/tmp/data.js
```
Exec js scripts:
```
docker compose exec mongodb mongosh /tmp/validation.js
```

```
docker compose exec mongodb mongosh /tmp/data.js
```
Run the command to see collections made: (or you can enter myservice_mongo container and run all commands there)
```
docker compose exec mongodb mongosh myservice_mongo --eval "show collections"
```