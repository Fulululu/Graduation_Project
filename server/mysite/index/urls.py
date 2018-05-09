from django.urls import path
from django.conf.urls import url
from . import views

app_name = 'index'

urlpatterns = [
    path('', views.index, name='index'),
    path('index/', views.index, name='index'),
    path('login/', views.login, name='login'),
    path('register/', views.register, name='register'),
    path('forgot/', views.forgot, name='forgot'),
    path('reset/', views.reset, name='reset'),
    path('mydata/', views.mydata, name='mydata'),
    path('logout/', views.logout, name='logout'),
    path('update/', views.update, name='update'),
]
