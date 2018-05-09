import random
import sqlite3
import time

def keygen(self):
    chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    code = ""
    length = len(chars)-1
    codelen = random.randint(6,9) 
    for i in range(0,codelen):
        idx = random.randint(0, length)
        code += chars[idx]
        now = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    return code,now

if __name__ == "__main__":
    code, now = keygen()
    try:
        conn = sqlite3.connect('db.sqlite3')
        cursor = conn.cursor()
        cursor.execute("INSERT INTO index_invitation (CODE, CREATETIME) VALUES ('{}','{}')".format(code, now))
        print("Create code: "+code)
    except:
        print("Fail to create code")
    finally:
        cursor.close()
        conn.commit()
        conn.close()
