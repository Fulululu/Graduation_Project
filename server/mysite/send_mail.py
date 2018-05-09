import os
from django.core.mail import send_mail

os.environ['DJANGO_SETTINGS_MODULE'] = 'mysite.settings'

if __name__ == '__main__':
    send_mail('来自xxx的测试邮件','Welcome!!!','ygfulululu@sina.com',['510924866@qq.com'],)
