package com.example.damon.notepad;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.EditText;

/**
 * Created by damon on 17-9-6.
 */

public class LoginListener implements View.OnClickListener,CompoundButton.OnCheckedChangeListener {
    //登录界面的activity
    LoginActivity loginActivity;
    MyDatabaseHelper databaseHelper;

    public boolean isSavedPassword() {
        return isSavedPassword;
    }

    public void setSavedPassword(boolean savedPassword) {
        isSavedPassword = savedPassword;
    }

    //是否保存密码
    private boolean isSavedPassword = false;
    public LoginListener(Context context){
        this.loginActivity = (LoginActivity) context;
        this.databaseHelper = new MyDatabaseHelper(loginActivity,"mydata.db3",1);
    }
    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            //点击登录按钮
            case R.id.login_btn:
                //获取输入的登录信息
                String loginName = loginActivity.userName.getText().toString();
                String loginPassword = loginActivity.password.getText().toString();
                //判断用户名是否存在
                if(!databaseHelper.isUserExists(loginName)){

                        new AlertDialog.Builder(loginActivity)
                                .setTitle("提示")
                                .setMessage("用户名不存在!")
                                .setPositiveButton("确定",null)
                                .show();
                        return ;

                }
                //登录成功
                else if(databaseHelper.isPasswordRight(loginName,loginPassword)){
                    //对于登录信息的处理.
                    dealWithLoginMessage();
                    Intent intent = loginActivity.getIntent();
                    intent.putExtra("userName",loginName);
                    loginActivity.setResult(LoginActivity.RESULT_CODE,intent);
                    loginActivity.finish();
                //密码错误
                }else {
                    new AlertDialog.Builder(loginActivity)
                            .setTitle("提示")
                            .setMessage("密码错误!")
                            .setPositiveButton("确定",null)
                            .show();
                    return ;
                }

                break;

            //点击下面的 注册按钮.
            case R.id.register_btn:
                //注册dialog的view
                View registerView = loginActivity.getLayoutInflater()
                        .inflate(R.layout.register_view,null);
                final EditText registerNameEdit = (EditText) registerView.
                        findViewById(R.id.rigister_user);
                final EditText registerPasswordEdit = (EditText) registerView.
                        findViewById(R.id.rigister_password);
                //以dialog形式简单注册即可.
                new AlertDialog.Builder(loginActivity)
                        .setTitle("注册")
                        .setView(registerView)
                        //确认注册按钮
                        .setPositiveButton("注册", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i) {
                                //获取用户输入的注册用户名和密码.
                                String registerName = registerNameEdit.getText().toString();
                                String registerPassword = registerPasswordEdit.getText().toString();
                                //判断用户名和密码是否合法
                                if(isUserValid(registerName)&&isPasswordValid(registerPassword)){

                                    //用户名已经存在
                                    if(databaseHelper.isUserExists(registerName)) {
                                        new AlertDialog.Builder(loginActivity)
                                                .setTitle("提示")
                                                .setMessage("用户名已存在!")
                                                .setPositiveButton("确定",null)
                                                .show();
                                        return ;
                                    }
                                    //更新数据库中的user表,为新增加的user新增一个表.
                                    databaseHelper.updateUser(registerName,registerPassword);
                                    databaseHelper.createSingleTable(registerName);
                                    new AlertDialog.Builder(loginActivity)
                                            .setTitle("提示")
                                            .setMessage("恭喜注册成功")
                                            .setPositiveButton("确定",null)
                                            .show();
                                }
                                else{
                                    new AlertDialog.Builder(loginActivity)
                                            .setTitle("提示")
                                            .setMessage("注册失败!请注意注册界面的提示信息!")
                                            .setPositiveButton("确定",null)
                                            .show();
                                }

                            }
                        })
                        .setCancelable(false)
                        .setNegativeButton("取消",null)
                        .show();
                break;
        }
    }
    //处理登录信息,是否保存密码.
    private void dealWithLoginMessage() {
        SharedPreferences preferences = loginActivity
                .getSharedPreferences(MainActivity.LOGIN_MESSAGE,Context.MODE_WORLD_WRITEABLE);
        SharedPreferences.Editor editor = preferences.edit();
        String loginName = loginActivity.userName.getText().toString();
        String loginPassword = loginActivity.password.getText().toString();
        editor.putInt(MainActivity.LOGIN_STATE,MainActivity.LOGIN_STATE_IN);
        //不管是否保存密码,登录名都要保存
        editor.putString(MainActivity.LOGIN_NAME,loginName);
        //如果要保存密码
        if(isSavedPassword){

            editor.putString(MainActivity.PASSWORD,loginPassword);

        }else{
            editor.putString(MainActivity.PASSWORD,"");
        }
        editor.commit();
    }

    //判断用户名是否合法,只能是数字.不大于10位
    private boolean isUserValid(final String user){
        if(user == null||user.length()>10||user.length()<1)return false;
        for(int i = 0;i < user.length();i++){
            if(user.charAt(i)>'9'||user.charAt(i)<'0') return false;
        }

        return true;
    }
    //注册时判断密码是否合法,只能包含数字和字母.不大于12位.不小于6位
    private boolean isPasswordValid(final String password){
        if(password == null ||password.length()>12||password.length()<6) return false;
        for(int i = 0;i < password.length();i++){
            char c = password.charAt(i);
            if(c<'0'||(c>'9'&&c<'A')||(c>'Z'&&c<'a')||c>'z')
                return false;
        }

        return true;
    }


    @Override
    public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
        if(b) {
            isSavedPassword = true;
        }
        else isSavedPassword = false;
    }
}
