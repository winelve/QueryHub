## AST

```json
-- base

{
    "status": "ok|error",
    "CMD":"CREARE|DROP|ALTER",
    "data":{
        "status":"ok|error",
        "error_log":""
    },
    "error_log":""
}


-- create:
{
    "data":{
        "object":"database",
        "name":"name"
    }
}

{
    "data":{
        "object":"table",
        "name":"name",
        "columns":[
            {
                "cname":"name",
                "ctype":"type",
                "constrain":"constrain",
            },
            {
				"cname":"name",
                "ctype":"type",
                "constrain":"constrain",
            }
        ]
    }
}

-- drop
{
    "data":{
        "object":"database",
        "name": "name"
    }
}

{
    "data":{
        "object":"table",
        "name": "name"
    }
}

--alter

rename-db
{
    "data":{
        "object": "database",
        "op": "rename",
        "name": "name",
        "new_name": "new_name"
    }
}

rename-tb
{
    "data":{
        "object": "table",
        "op": "rename",
        "name": "name",
        "new_name": "new_name"
    }
}

rename-column
{
    "data":{
        "object": "column",
        "op": "rename",
        "table_name":"name",
        "column_name": "name",
        "new_name": "name"
    }
}




add-column
{
    "data": {
    "object":"column",
    "op":"add",
    "table_name": "name",
    "columns": [
       {
            "cname":"name",
	        "ctype":"type",
    	    "constrain","constrain"
       },
	   {
            "cname":"name",
	        "ctype":"type",
    	    "constrain","constrain"
       }
    ]
    }
}

md-column
{
    "data":{
        "object":"column",
        "op":"modify",
        "table_name":"name",
        "column_name":"name",
        "new_type":"type",
    }
}



drop-column
{
    "data":{
        "object":"column",
        "op": "drop",
        "table_name": "name",
        "column_name": "cname",
        "type": "type"
    }
    
}





// normal
{
    "cmd":"",
    "status":"",
    "error_log":"",
    "data": {
        
    }
    
}


```



```json

这个 Json数据 用来 自动维护参数

{
    "cmds":[
        {
            "cmd": "alter",
            "op": {
                "rename": {
                    "object": {
                        "database": {
                            "func":["AlterDatabaseName"],
                            "keys": ["name", "new_name"]
                        },
                        "table": {
                            "func":["AlterTableName"],
                            "keys": ["name", "new_name"]
                        },
                        "column": {
                            "func":["AlterColumnName"],
                            "keys": ["table_name", "column_name", "new_name"]
                        }
                    }
                },
                "add": {
                    "object": { 
                        "column": {
                            "func": ["AlterTableAdd","AlterTableConstraint"],
                            "keys": ["table_name", "columns"]
                        }
                    }
                },
                "modify": {
                    "object": {
                        "column": {
                            "func": ["AlterTableModify"],
                            "keys": ["table_name", "column_name", "new_type"]
                        }
                    }
                },
                "drop": {
                    "object": {
                        "column": {
                            "func":["AlterTableDrop"],
                            "keys": ["table_name", "column_name"]
                        }
                    }
                }
            }
            
        },
        
        {
            "cmd": "create",
            "object": {
                "database": {
                    "func":["CreateDatabase"],
                    "keys": ["name"]
                },
                "table": {
                    "func":["CreateTable"],
                    "keys": ["name", "columns"]
                }
            }
        },
        
        {
            "cmd": "drop",
            "object": {
                "database": {
                    "func":["DeleteDatabase"],
                    "keys": ["name"]
                    
                },
                "table": {
                    "func":["DropTable"],
                    "keys": ["name"]
                }
            }
        }

    ]
}
```



