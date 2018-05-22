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
                    '''.format(code, '45.77.171.135:8000')

    msg = EmailMultiAlternatives(subject, text_content, settings.EMAIL_HOST_USER, [emailto])
    msg.attach_alternative(html_content, "text/html")
    msg.send()

def logout(request):
    if not request.session.get('is_login', None):
        return redirect("/login")
    request.session.flush()
    return redirect("/index/")

def update(request, opt):
    if request.session.get('is_login', None):
        if opt == 0:
            summary_objs = models.summary.objects.filter(UID_id=request.session['user_id'])
            if summary_objs:  # the method 'latest' will raise error when object is empty(so need 'if'), but 'only' method not.
                tmp_objs = summary_objs
                obj_list = []
                while True:
                    obj_list.append(tmp_objs.latest('CREATETIME'))
                    tmp_objs = tmp_objs.exclude(NODE=obj_list[-1].NODE)
                    if not tmp_objs:
                        break
                    obj_list.sort(key=lambda x:x.NODE)

                data_list = []
                for item in obj_list:
                    tmp_dict = {'UID_id':item.UID_id,\
                                'NODE':item.NODE,\
                                'LIGHT':item.LIGHT,\
                                'AIRTEMP':item.AIRTEMP,\
                                'AIRHUMI':item.AIRHUMI,\
                                'SOILTEMP':item.SOILTEMP,\
                                'SOILHUMI':item.SOILHUMI}
                    data_list.append(tmp_dict)
                retjson = json.dumps(data_list)
        elif opt == 1:
            dev_state = models.device.objects.get(UID_id=request.session['user_id'])
            dev_dict = {'pump_state':dev_state.PUMP, 'lamp_state':dev_state.LAMP}
            dev_list = []
            dev_list.append(dev_dict)
            retjson = json.dumps(dev_list)
        else:
            return 1
        response = HttpResponse()
        response['Content-Type'] = "text/javascript"
        response.write(retjson)
        return response
    return 1

def devctl(request, dev_id):
    if request.session.get('is_login', None):
        dev_state = models.device.objects.get(UID_id=request.session['user_id'])
        if(dev_id == 0):
            dev_state.PUMP = not dev_state.PUMP
            dev_state.AUTO_PUMP = not dev_state.AUTO_PUMP
            dev_dict = {'pump_state':dev_state.PUMP}
        elif(dev_id == 1):
            dev_state.LAMP = not dev_state.LAMP
            dev_state.AUTO_LAMP = not dev_state.AUTO_LAMP
            dev_dict = {'lamp_state':dev_state.LAMP}
        dev_state.save()
        dev_list = []
        dev_list.append(dev_dict)
        retjson = json.dumps(dev_list)
        response = HttpResponse()
        response['Content-Type'] = "text/javascript"
        response.write(retjson)
        return response
    

# Create your views here.

def index(request):
    pass
    return render(request, 'index.html')

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
            
        models.user.objects.create(ACCOUNT=account, PASSWORD=password, EMAIL=email)
        new_UID = models.user.objects.order_by('-CREATETIME')[0].UID
        models.device.objects.create(UID_id=new_UID, LAMP=False, PUMP=False, AUTO_LAMP=True, AUTO_PUMP=True)
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
        summary_objs = models.summary.objects.filter(UID_id=request.session['user_id'])
        if summary_objs:  # the method 'latest' will raise error when object is empty(so need 'if'), but 'only' method not.
            tmp_objs = summary_objs
            node_list = []
            while True:
                node_list.append(tmp_objs.latest('CREATETIME'))
                tmp_objs = tmp_objs.exclude(NODE=node_list[-1].NODE)
                if not tmp_objs:
                    break
            node_list.sort(key=lambda x:x.NODE)

        light_objs = summary_objs.only('CREATETIME','UID_id','NODE','LIGHT')
        airtemp_objs = summary_objs.only('CREATETIME','UID_id','NODE','AIRTEMP')
        airhumi_objs = summary_objs.only('CREATETIME','UID_id','NODE','AIRHUMI')
        soiltemp_objs = summary_objs.only('CREATETIME','UID_id','NODE','SOILTEMP')
        soilhumi_objs = summary_objs.only('CREATETIME','UID_id','NODE','SOILHUMI')
        return render(request, 'data.html',locals())
    
    return render(request, 'tologin.html')

