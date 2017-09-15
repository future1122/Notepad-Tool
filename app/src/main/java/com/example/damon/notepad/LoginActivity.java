package com.example.damon.notepad;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.KeyEvent;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;

public class LoginActivity extends Activity {
    final static int RESULT_CODE = 1;

    //登录界面的按钮监听.
    MyDatabaseHelper databaseHelper;
    LoginListener loginListener;
    EditText userName;
    EditText password;
    Button login;
    Button register;
    CheckBox rememberPassword;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);
        //找到 layout 组件
        initComp();
        //设置按钮的监听
        setListener();
    }

    private void initComp() {
        databaseHelper = new MyDatabaseHelper(this,"mydata.db3",1);
        loginListener = new LoginListener(this);
        SharedPreferences preferences =
                getSharedPreferences(MainActivity.LOGIN_MESSAGE, Context.MODE_WORLD_READABLE);
        String loginName = preferences.getString(MainActivity.LOGIN_NAME,"");
        String loginPassword = preferences.getString(MainActivity.PASSWORD,"");
        userName = (EditText) findViewById(R.id.user_edit);
        password = (EditText) findViewById(R.id.password_edit);
        userName.setText(loginName);
        password.setText(loginPassword);
        login = (Button) findViewById(R.id.login_btn);
        register = (Button) findViewById(R.id.register_btn);
        rememberPassword = (CheckBox) findViewById(R.id.remember_password);
        //
        if(loginPassword != null && !loginPassword.equals("")) {
            loginListener.setSavedPassword(true);
            rememberPassword.setChecked(true);
        }
    }

    private void setListener() {
        login.setOnClickListener(loginListener);
        register.setOnClickListener(loginListener);
        rememberPassword.setOnCheckedChangeListener(loginListener);
    }


    //登录界面时将返回键取消避免 未登录就能进入程序.
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if(keyCode == KeyEvent.KEYCODE_BACK) return true;
        return super.onKeyUp(keyCode, event);
    }
}
