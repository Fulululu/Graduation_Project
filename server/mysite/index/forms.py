from django import forms


class UserForm(forms.Form):
    account = forms.CharField(label="帐号", max_length=25, widget=forms.TextInput(attrs={'class': 'form-control'}))
    password = forms.CharField(label="密码", max_length=25, widget=forms.PasswordInput(attrs={'class': 'form-control'}))
