# Intro
This is an NGINX module to check for a valid JWT and proxy to an upstream server.

## Installation
```
cd nginx-1.x.x
./configure --add-module=/path/to/ngx_jwt
make && make install
```

## Configuration directives
### `ngx_jwt`
- **syntax**: `ngx_jwt`
- **context**: `server`, `location`

## Reverse Proxy Basics
From your local, you can hit a Spring Boot container with CURL.
Assuming we mapped the container to port 8084 on our Docker Machine, we use:
```bash
curl 192.168.99.100:8084 --cookie "foo=someValue" --header "bar: 123"
```

But to test that our NGINX reverse proxy is working, we want to hit Docker Machine's port 8083, since it maps NGINX's port 80.
```bash
curl 192.168.99.100:8083 --cookie "foo=hu" --header "bar: 123"
curl 192.168.99.100:8083/boot1 --cookie "foo=hu" --header "bar: 123"
curl 192.168.99.100:8083/boot2 --cookie "foo=hu" --header "bar: 123"
```

## Learning Docker
### Docker Basics
Start your VM that will be hosting your containers. Mine is called "default"
```bash
docker-machine start default
```

Load your VM's environmental variables into your shell 
```bash
eval "$(docker-machine env default)"
```

Shell into your NGINX container
```bash
docker exec -it -u root <CONTAINER_1_NAME> /bin/bash
```

Docker Compose links containers, so if you're running bash from within CONTAINER_1, the following will work:
```
root@ae6a38525ffb:/# ping CONTAINER_2
```

### Rebuilding Docker images
```bash
# Run containers
gradle clean && gradle assemble && docker-compose up

# Show all containers
docker ps --all

# Stop containers
docker-compose stop

# Stop a specific container
docker-compose stop nginx

# Delete all images
docker rmi --force $(docker images -q) 
docker-compose rm --force nginx
docker-compose rm --force boot1
docker-compose rm --force boot2

# Rebuild
docker-compose build --no-cache
```
