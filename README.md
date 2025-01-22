# MHD Postgres API

a simple RESTful API built with microhttpd that communicates with a Postgres server, with full CRUD capabilities

comments are simply my annotations since this project is mainly for educational purposes

data is monkeys from bloons td 6 because why not

prerequisites: libmicrohttpd, postgresql, cJSON

compile with:
```
gcc -I./include src/*.c src/actions/*.c -o a.out -lmicrohttpd -lpq -lcjson
```

# Routes

requests and responses documented below

## Get all monkeys

### Request

`GET /monkeys`

```
curl --request GET \
  --url 'http://localhost:5000/monkeys?='
```

### Response

```
{
    "monkeys": [
        {
			"id": integer,
			"name": "string",
			"price": integer,
			"type": "char"
		}
    ]
}
```

"type" refers to primary, magic, military or support ("P", "M" and so on, deal with the double "M" however you like)

## Get monkey by Id

### Request

`GET /monkey/{id}`

```
curl --request GET \
  --url 'http://localhost:5000/monkey/1?='
```

### Response

```
{
    "monkey": {
        "id": integer,
        "name": "string",
        "price": integer,
        "type": "char"
    }
}
```

## Create monkey

### Request

`POST /monkey`

```
curl --request POST \
  --url 'http://localhost:5000/monkey/?=' \
  --header 'Content-Type: application/json' \
  --data '{
	"monkey": {
		"name": "string",
		"price": integer,
		"type": "char"
	}
}'
```

### Response

```
{
    "monkey": {
        "id": integer,
        "name": "string",
        "price": integer,
        "type": "char"
    }
}
```

will return the same data that was sent + the row id

## Update monkey

### Request

`PUT /monkey/{id}`

```
curl --request PUT \
  --url 'http://localhost:5000/monkey/10?=' \
  --header 'Content-Type: application/json' \
  --data '{
	"monkey": {
        "name": "string",
        "price": integer,
        "type": "char"
    }
}'
```

### Response

```
update successfull
```

## Delete monkey

### Request

`DELETE /monkey/{id}`

```
curl --request DELETE \
  --url 'http://localhost:5000/monkey/10?='
```

### Response

```
delete successfull
```

## Get image (bonus route)

### Request

`GET /fusty`

```
curl --request GET \
  --url 'http://localhost:5000/fusty?='
```

### Response

an image of pat fusty T-posing, found in the assets folder. just a bonus route to showcase file reading and sending capabilities

# Planned features

- some form of authentication
- additional search routes (e.g. by name, price range, etc.)