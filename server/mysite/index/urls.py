from django.urls import path

from . import views

app_name = 'index'

urlpatterns = [
    path('', views.index, name='index'),
    path('login/', views.login, name='login'),
    path('register/', views.register, name='register'),
    path('forgot/', views.forgot, name='forgot'),
    path('reset/', views.reset, name='reset'),
    path('sensor/', views.sensor, name='sensor'),
]
