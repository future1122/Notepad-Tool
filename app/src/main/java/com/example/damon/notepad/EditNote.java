package com.example.damon.notepad;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by damon on 17-9-8.
 * 编辑note的activity
 */

public class EditNote extends Activity {
    final static int RESULT_CODE = 2;
    //要返回给主界面的标题和内容,
    String title = "";
    String content = "";

    EditText titleEdit,contentEdit;
    Button exitEdit,save;
    Intent intent;
    //是否进行过保存.
    boolean isSaved = false;
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.edit_note);
        initComps();
        //为保存按钮设置监听
        save.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(titleEdit.getText().toString()==null||titleEdit.getText().toString().equals(""))
                {
                    Toast.makeText(EditNote.this, "标题不可以为空", Toast.LENGTH_SHORT).show();
                    return;
                }
                if(titleEdit.getText().toString().equals(title)&&
                        contentEdit.getText().toString().equals(content)){
                    return ;
                }
                title = titleEdit.getText().toString();
                content = contentEdit.getText().toString();
                isSaved = true;
                Toast.makeText(EditNote.this, "保存成功!", Toast.LENGTH_LONG).show();
            }
        });
        //退出按钮设置监听
        exitEdit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                    //已经保存了就直接退出.
                    if(titleEdit.getText().toString().equals(title)
                            &&contentEdit.getText().toString().equals(content))
                    {
                        //exit表名要发送数据到主界面 并更新
                        if(isSaved)exit();
                        //直接退出不做任何改动
                        else EditNote.this.finish();
                    }
                    else{
                        //没有保存新建对话框提示用户是否保存
                        new AlertDialog.Builder(EditNote.this)
                                .setTitle("温馨提示!")
                                .setMessage("您的记事信息尚未保存,确定退出吗?")
                                .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialogInterface, int i) {
                                        //exit表名要发送数据到主界面 并更新
                                        if(isSaved)exit();
                                            //直接退出不做任何改动
                                        else EditNote.this.finish();
                                    }
                                })
                                .setNegativeButton("否",null)
                                .show();
                    }

            }
        });
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if(event.getKeyCode()==KeyEvent.KEYCODE_BACK)return false;
        return super.onKeyUp(keyCode, event);
    }

    private void exit() {
        //获取当前修改日期
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm");
        String date = sdf.format(new Date());
        intent.putExtra(MainActivity.TITLE,title);
        intent.putExtra(MainActivity.CONTENT,content);
        intent.putExtra(MainActivity.DATE,date);
        EditNote.this.setResult(RESULT_CODE,intent);
        EditNote.this.finish();
    }


    private void initComps() {
        intent = getIntent();
        title = intent.getStringExtra(MainActivity.TITLE);
        content = intent.getStringExtra(MainActivity.CONTENT);
        titleEdit = (EditText)findViewById(R.id.title_edit);
        contentEdit = (EditText) findViewById(R.id.content_edit);
        titleEdit.setText(title);
        contentEdit.setText(content);
        exitEdit = (Button) findViewById(R.id.exit_edit);
        save = (Button) findViewById(R.id.save);
    }
}
