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

//--------------DDL--------------
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
        "columns": [
            {
               	"cname":"name",
                "ctype":"type",
                "constraints": [
                    {
                        "csname":"csname",
                        "params":[]
                    },
                ]
           },
           {
               "cname":"name",
                "ctype":"type",
                "constraints": [
                    {
                        "csname":"csname",
                        "params":[]
                    },
                ]
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
    	    "constraints": [
                {
            		"csname":"csname",
               		"params":[]
           		},
            ]
       },
	   {
           "cname":"name",
	        "ctype":"type",
    	    "constraints": [
                {
            		"csname":"csname",
               		"params":[]
           		},
            ]
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

add-cs
{
    "data": {
        "op": "add",
        "object": "constraint",
        "tb_name":"tb_name",
        "cname":"cname",
        "constraints": [
                {
            		"csname":"csname",
               		"params":[]
           		},
            ]
    }   
}


drop-cs
{
    "data": {
        "op":"drop",
        "object": "constraint",
        "tb_name":"tb_name",
        "cname":"cname",
        "cs_name":"name
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

//对于foregin key的约束
foregin_key(reference_tb,reference_column)

//other cmds
//use database
{
    "cmd":"use",
    "data": {
        "op":"use",
        "object":"database",
        "db_name": "name"
    }
    
}


//show database

{
    "cmd":"show",
    "data": {
        "op":"show",
        "object":"database",
    }
    
}

//show tables
{
    "cmd":"show",
    "data": {
        "op":"show",
        "object":"table",
    }
}

// Show Constraints
{
    "cmd":"show",
    "data": {
        "op":"show",
        "object":"constraints",
    }
    
}
//desc table
{
    "cmd": "desc",
    "data": {
        "op":"desc",
        "object":"table",
        "name": "name"
    }
}

//--------------DML--------------

// Insert
// INSERT INTO table_name (column1,column2,column3) VALUES (v11,v12,v13),(v21,v22,v23); 
{
    "cmd": "insert",
    "data": {
        "op":"insert",
        "object": "table",
        "tb_name":"tb_name",
        "columns": [],
        "values":[]
    }
}


//SELECT
//SELECT (c1,c2,c3,c4|*) FROM tb1 WHERE (c1>10 AND c2<100) JOIN (tb2,tb3) ORDERED BY (c1,c2...);

{
    "cmd":"select",
    "data": {
        "op":"select",
        "object": "table",
        "tables": ["tb_name"],
        "select_columns":[],
        "order_by":[],
        "where":[
            {
                "left":"l",
                "right":"r",
                "condition":"c"
            }
        ]
    }
}

//UPDATE
//UPDATE tb_name SET (c1=v1,c2=v2,c3=v3) WHERE (c1>10 AND c2<100);
{
    "cmd":"update",
    "data": {
        "op":"update",
        "object": "table",
        "tb_name": "name",
        "set_values": [
            {
                "cname":"cname",
                "values":"value"
            }
        ],
        "where":[
            {
                "left":"l",
                "right":"r",
                "condition":"c"
            }
        ]
    }
}

//DELETE
//DELETE tb_name WHERE (c1<10 AND c2>100)
{
    "cmd": "delete",
    "data": {
        "op": "delete",
        "object": "table",
        "tb_name": "name",
        "where":[
            {
                "left":"l",
                "right":"r",
                "condition":"c"
            }
        ]
    }
}


//----------------COMMIT

{
    "cmd": "commit",
    "data": {
        "op":"commit",
        "object": "database"
    }
}

{
    "cmd":"rollback",
    "data": {
        "op":"rollback",
        "object": "database"
    }
}



//-------------------INDEX
//create index on tb cname;
//drop index on tb cname;
{
    "cmd": "create",
    "data": {
        "op":"index",
        "object": "table",
        "tb_name": "name",
        "cname": "name"
    }
}

{
    "cmd": "drop",
    "data": {
        "op":"index",
        "object": "table",
        "tb_name": "name",
        "cname": "name"
    }
}


-----LOGIN------------
// login username p pwd;
{
    "cmd": "login",
    "data": {
        "op":"login",
        "object": "database",
        "username": "",
        "pwd": ""
    }
}

// register wyq 123456;
{
    "cmd": "register",
    "data": {
        "op": "register",
        "object": "user",
        "username": "name",
        "pwd": "pwd"
    }
}



//----------GRANT--------------
// GRANT 	OWNER	 ON	 db_name 	TO	 user_name
{
    "cmd": "grant",
    "data": {
        "op":"grant",
        "object": "database",
        "db_name": "name",
        "username": "name"
    }
}

// ----------------grant----------

{
    "cmd": "grant",
    "data": {
        "op":"grant",
        "object": "table",
        "power":["SELECT","..."],
        "db_name": "name",
        "tb_name": "name",
        "username": "name"
    }
}


// REVOKE	OWNER 	ON	db_name	[FROM	user_name]
{
    "cmd": "revoke",
    "data": {
        "op":"revoke",
        "object": "database",
        "db_name": "name",
        "username": "name"
    }
}
//REVOKE        (a1,a2,a3 | *)        ON         (db_name.tb_name | db_name.* | *.*)   FROM user_name
{
    "cmd": "revoke",
    "data": {
        "op":"revoke",
        "object": "table",
        "power":["SELECT","..."],
        "db_name": "name",
        "tb_name": "name",
        "username": "name"
}





```

## JsonData

```json

这个 Json数据 用来 自动维护参数

{
    "cmds":[
        {
            "cmd": "alter",
            "rename": {
                "database": {
                    "func":["AlterDatabaseName"],
                    "keys": ["name", "new_name"]
                },
                "table": {
                    "func":["AlterTableName"],
                    "keys": ["name", "new_name"]
                },
                "column": {
                    "func":["AlterTableColumnName"],
                    "keys": ["table_name", "column_name", "new_name"]
                }
            },
            "add": {
                "column": {
                    "func": ["AlterTableAdd","AlterTableConstraint"],
                    "keys": ["table_name", "columns"]
                },
                "constraint": {
                    "func":["AlterTableConstraint"],
                    "keys":["tb_name","cname","constraints"]
                }
            },
            "modify": {
                "column": {
                    "func": ["AlterTableModify"],
                    "keys": ["table_name", "column_name", "new_type"]
                }
            },
            "drop": {
                "column": {
                    "func":["AlterTableDrop"],
                    "keys": ["table_name", "column_name"]
                },
                "constraint": {
                    "func":["AlterTableDeleteConstraint"],
                    "keys":["tb_name","cname","cs_name"]
                },
                
                
            }
        },
        
        {
            "cmd": "create",
            "create": {
                "database": {
                    "func":["CreateDatabase"],
                    "keys": ["name"]
                },
                "table": {
                    "func":["CreateTable"],
                    "keys": ["name", "columns"]
                },
				"index": {
                	"table": {
                    	"func": ["BuildIndex"],
                    	"keys": ["tb_name", "cname"]
                	}
            	}  
            }
        },
        
        {
            "cmd": "drop",
            "drop": {
                "database": {
                    "func":["DeleteDatabase"],
                    "keys": ["name"]
                    
                },
                "table": {
                    "func":["DropTable"],
                    "keys": ["name"]
                },
				"index": {
                    "table": {
                        "func": ["DestroyIndex"],
                        "keys": ["tb_name", "cname"]
                    }
                }
            }
        },

        {
            "cmd":"use",
            "use": {
                    "database": {
                    "func": ["UseDatabase"],
                    "keys": ["db_name"]
                }
            }

        },
        
        {
            "cmd":"show",
            "show": {
                "database": {
						"func": ["ShowDatabases"],
                     "keys": []
                },
                "table": {
                    "func": ["ShowTables"],
                    "keys": []
                },
                "constraints": {
                    "func": ["ShowConstraints"],
                    "keys": []
                }
            }
        },
        
        {
            "cmd":"desc",
            "desc": {
                "table": {
                    "func":["DescribeTable"],
                    "keys":["name"]
                }
            }
        },
        
        {
            "cmd":"insert",
            "insert": {
                "table": {
                    "func":["insert"],
                    "keys":["tb_name","columns","values"]
                }
            }
        },
        
        {
            "cmd": "select",
            "select": {
                "table":{
                    "func":["Select"],
                    "keys":["tables","select_columns","where","order_by"]
                }
            }
        },
        
        {
            "cmd":"update",
            "update": {
                "table":{
                    "func":["Update"],
                    "keys":["tb_name","set_values","where"]
                }
            }
        },
        
        {
            "cmd": "delete",
            "delete": {
                "table": {
                    "func":["Delete"],
                    "keys":["tb_name","where"]
                }
                
            }
            
            
        },
        //-----------------------------------------------------------
        
        {
            "cmd": "commit",
            "commit": {
                "database": {
                    "func": ["Write"],
                    "keys": []
                }
            }
        },
        
        {
            "cmd": "rollback",
            "rollback": {
                "database": {
                    "func": ["Read"],
                    "keys": []
                }
            }
        },
        
//--------------------------
        
        
        {
            "cmd": "login",
            "login": {
                "database": {
                    "func": ["Login"],
                    "keys": ["username", "pwd"]
                }
            }
        },
        
        {
          	"cmd": "register",
            "register": {
                "user": {
                    "func":["CreateUser"],
                    "keys":["username","pwd"]
                }
            }
        },
        
        {
            "cmd": "grant",
            "grant": {
                "database": {
                    "func": ["GrantDatabaseOwner"],
                    "keys": ["db_name", "username"]
                },
                "table": {
                    "func": ["GrantAuthority"],
                    "keys": [ "username", "db_name", "tb_name","power"]
                }
            }
        },
        
        {
            "cmd": "revoke",
            "revoke": {
                "database": {
                    "func": ["RevokeDatabaseOwner"],
                    "keys": ["db_name", "username"]
                },
                "table": {
                    "func": ["RevokeAuthority"],
                    "keys": ["username", "db_name", "tb_name", "power"]
                }
            }
        }
    

    ]
}

////--------------------------------------






```

