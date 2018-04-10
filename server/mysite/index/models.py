from django.db import models
from django.utils import timezone

# Create your models here.

class invitation(models.Model):
    CODE = models.CharField(max_length=10)
    CREATETIME = models.DateTimeField(default=timezone.now)

class resetcode(models.Model):
    ACCOUNT = models.CharField(max_length=30)
    RESETCODE = models.CharField(max_length=10,null=True)
    CREATETIME = models.DateTimeField(default=timezone.now)
    
class user(models.Model):
    UID = models.IntegerField(primary_key=True)
    ACCOUNT = models.CharField(max_length=30)
    PASSWORD = models.CharField(max_length=30)
    EMAIL = models.CharField(max_length=30)
    CREATETIME = models.DateTimeField(default=timezone.now)

class device(models.Model):
    UID = models.ForeignKey(user, on_delete=models.CASCADE)
    LAMP = models.BooleanField(max_length=1)
    PUMP = models.BooleanField(max_length=1)

class light(models.Model):
    UID = models.ForeignKey(user, on_delete=models.CASCADE)
    NODE = models.IntegerField()
    DATA = models.IntegerField()
    CREATETIME = models.DateTimeField(auto_now=True)

class airtemp(models.Model):
    UID = models.ForeignKey(user, on_delete=models.CASCADE)
    NODE = models.IntegerField()
    DATA = models.FloatField(max_length=5)
    CREATETIME = models.DateTimeField(auto_now=True)

class airhumi(models.Model):
    UID = models.ForeignKey(user, on_delete=models.CASCADE)
    NODE = models.IntegerField()
    DATA = models.IntegerField()
    CREATETIME = models.DateTimeField(auto_now=True)
    
class soiltemp(models.Model):
    UID = models.ForeignKey(user, on_delete=models.CASCADE)
    NODE = models.IntegerField()
    DATA = models.FloatField(max_length=5)
    CREATETIME = models.DateTimeField(auto_now=True)

class soilhumi(models.Model):
    UID = models.ForeignKey(user, on_delete=models.CASCADE)
    NODE = models.IntegerField()
    DATA = models.IntegerField()
    CREATETIME = models.DateTimeField(auto_now=True)

class co2(models.Model):
    UID = models.ForeignKey(user, on_delete=models.CASCADE)
    NODE = models.IntegerField()
    DATA = models.FloatField(max_length=5)
    CREATETIME = models.DateTimeField(auto_now=True)
