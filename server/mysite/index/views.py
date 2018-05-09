from django.http import HttpResponse, HttpResponseRedirect
from django.shortcuts import render, redirect
from index import models
from django.core.mail import EmailMultiAlternatives
from django.conf import settings
import random
import sqlite3
from django.utils import timezone
from django.views.decorators.csrf import requires_csrf_token
import json

# Build-in function  

def rstcode_keygen(account):
    chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    code = ""
    length = len(chars)-1
    codelen = random.randint(6,9) 
    for i in range(0,codelen):
        idx = random.randint(0, length)
        code += chars[idx]
        now = timezone.now()#.strftime("%Y-%m-%d %H:%M:%S")
    resetcode = models.resetcode.objects.create()
    resetcode.ACCOUNT = account
    resetcode.RESETCODE = code
    resetcode.CREATETIME = now
    resetcode.save()
    return code
    
def send_email(emailto, code):
    subject = '密码重置邮件'
    text_content = '''服务器异常，请稍后再尝试重置密码或联系管理员！'''
    html_content = '''
                    <p>您的重置码已生成:{}</p>
                    <p><a href="http://{}/reset" target=blank>密码重置链接</a></p>
                    <p>请点击站点链接完成重置！</p>
                    <p>此链接有效期为1天！</p>
                    '''.format(code, '127.0.0.1:8000')

    msg = EmailMultiAlternatives(subject, text_content, settings.EMAIL_HOST_USER, [emailto])
    msg.attach_alternative(html_content, "text/html")
    msg.send()

def logout(request):
    if not request.session.get('is_login', None):
        return redirect("/login")
    request.session.flush()
    return redirect("/index/")

def update(request):
    if request.session.get('is_login', None):
        newest_data = models.summary.objects.filter(UID_id=request.session['user_id'])[0]
        data_dict = {'UID_id':newest_data.UID_id,\
                     'NODE':newest_data.NODE,\
                     'LIGHT':newest_data.LIGHT,\
                     'AIRTEMP':newest_data.AIRTEMP,\
                     'AIRHUMI':newest_data.AIRHUMI,\
                     'SOILTEMP':newest_data.SOILTEMP,\
                     'SOILHUMI':newest_data.SOILHUMI}
        data_list = []
        data_list.append(data_dict)
        retjson = json.dumps(data_list)
        response = HttpResponse()
        response['Content-Type'] = "text/javascript"
        response.write(retjson)
        return response
    

# Create your views here.

def index(request):
    pass
    return HttpResponse("This the main page")

def login(request):
    if request.session.get('is_login',None):
        return redirect("/mydata/")
    if request.method == "POST":
        account = request.POST.get("account", None)
        password = request.POST.get("password", None)
        message = "帐号和密码不能为空！"
        if account and password:
            try:
                user = models.user.objects.get(ACCOUNT=account)
                if user.PASSWORD == password:
                    request.session['is_login'] = True
                    request.session['user_id'] = user.UID
                    request.session['user_account'] = user.ACCOUNT
                    request.session.set_expiry(0)
                    return redirect('/mydata/')
                else:
                    message = "密码不正确,请重新输入！"
            except:
                message = "帐号不存在！"
        return render(request, 'login.html', locals())
            
    return render(request, 'login.html', locals())

def register(request):
    if request.method == "POST":
        #message = "请检查填写的内容！"
        account = request.POST.get("account", None)
        password = request.POST.get("password", None)
        confirm = request.POST.get("confirm", None)
        email = request.POST.get("email", None)
        code = request.POST.get("code", None)
        if password != confirm:
            message = "两次输入的密码不同！"
            return render(request, 'register.html', locals())

        same_account = models.user.objects.filter(ACCOUNT=account)
        if same_account:  #this account is unique?
            message = '用户已经存在，请重新选择用户名！'
            return render(request, 'register.html', locals())

        same_email = models.user.objects.filter(EMAIL=email)
        if same_email:  # this e-mail is unique?
            message = '该邮箱地址已被注册，请使用别的邮箱！'
            return render(request, 'register.html', locals())

        same_code = models.invitation.objects.filter(CODE=code)
        if not same_code:  # this invitation code is valid?
            message = "该邀请码无效"
            return render(request, 'register.html', locals())
            
        user = models.user.objects.create()
        user.ACCOUNT = account
        user.PASSWORD = password
        user.EMAIL = email
        user.save()
        device = models.device.objects.create()
        device.UID_id = user.UID
        device.LAMP = False
        device.PUMP = False
        device.save()
        models.invitation.objects.filter(CODE=code).delete()
        
        return redirect('/login/')
    
    return render(request, 'register.html')

def forgot(request):
    if request.method == "POST":
        account = request.POST.get("account", None)
        email = request.POST.get("email", None)
        
        account_exist = models.user.objects.filter(ACCOUNT=account)
        if not account_exist:  #this account exist?
            message = '该帐号不存在！'
            return render(request, 'forgot.html', locals())

        match = models.user.objects.filter(EMAIL=email, ACCOUNT=account)
        if not match:  # this e-mail is unique?
            message = '输入的邮箱与帐号绑定的邮箱不一致！'
            return render(request, 'forgot.html', locals())

        send_email(email, rstcode_keygen(account))
        return redirect('/reset/')

    return render(request, 'forgot.html')

def reset(request):
    if request.method == "POST":
        rstcode = request.POST.get("rstcode", None)
        newpwd = request.POST.get("newpwd", None)

        if not rstcode:
            message = '必须要有重置码！'
            return render(request, 'reset.html', locals())
        
        code_item = models.resetcode.objects.filter(RESETCODE=rstcode)
        if not code_item:  #this code exist?
            message = '重置码错误或过期！'
            return render(request, 'reset.html', locals())
        
        if not newpwd:
            message = '新密码不能为空！'
            return render(request, 'reset.html', locals())
        
        models.user.objects.filter(ACCOUNT=code_item[0].ACCOUNT).update(PASSWORD=newpwd)
        models.resetcode.objects.filter(RESETCODE=rstcode).delete()
        return redirect('/login/')    
    
    return render(request, 'reset.html')

def mydata(request):
    if request.session.get('is_login', None):
        devices = models.device.objects.get(UID_id=request.session['user_id'])
        summary_list = models.summary.objects.filter(UID_id=request.session['user_id'])
        realtime_data = summary_list[0]
        light_list = models.summary.objects.filter(UID_id=request.session['user_id']).only('CREATETIME','UID_id','NODE','LIGHT')
        airtemp_list = models.summary.objects.filter(UID_id=request.session['user_id']).only('CREATETIME','UID_id','NODE','AIRTEMP')
        airhumi_list = models.summary.objects.filter(UID_id=request.session['user_id']).only('CREATETIME','UID_id','NODE','AIRHUMI')
        soiltemp_list = models.summary.objects.filter(UID_id=request.session['user_id']).only('CREATETIME','UID_id','NODE','SOILTEMP')
        soilhumi_list = models.summary.objects.filter(UID_id=request.session['user_id']).only('CREATETIME','UID_id','NODE','SOILHUMI')
        return render(request, 'data.html',locals())
    
    return render(request, 'tologin.html')


