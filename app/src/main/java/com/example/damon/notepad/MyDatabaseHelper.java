package com.example.damon.notepad;

import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by damon on 17-9-6.
 */

public class MyDatabaseHelper extends SQLiteOpenHelper {
    // 已经创建的数据库
    SQLiteDatabase db;

    final static String CREATE_USER_TABLE_SQL = "create table user(_id integer primary" +
            " key autoincrement , user_name, password)";


    public MyDatabaseHelper(Context context, String name, int version) {
        super(context, name, null, version);
        //获取用户创建的数据库.
        db = this.getWritableDatabase();

    }

    @Override
    public void onCreate(SQLiteDatabase sqLiteDatabase) {
        //第一次运行直接创建user表
        sqLiteDatabase.execSQL(CREATE_USER_TABLE_SQL);
    }

    @Override
    public void onUpgrade(SQLiteDatabase sqLiteDatabase, int i, int i1) {

    }
    //从user表中搜寻 user 是否存在
    public boolean isUserExists(final String user){
        Cursor cursor = db.rawQuery("select * from user where user_name = ?",
                new String[]{user});
        if(cursor.getCount() == 0) return false;
        return true;
    }

    //增加指定登录用户的记事条目信息,并返回_id-1值 也就是SharedPreferences的key
    public String addNote(String loginName,Notes note){
        db.execSQL("insert into user_"+loginName+" values(null,?,?)",new String[]
                {note.date,note.title});
        Cursor cursor = db.rawQuery("select last_insert_rowid() from user_"+
            loginName,null);
        int a = -1;
        if(cursor.moveToFirst()) a = cursor.getInt(0);
        return (a-1)+"";

    }

    //删除指定登录用户的记事条目信息
    public void deleteNote(String loginName,Notes note){
        db.execSQL("delete from user_"+loginName+" where _id = "+
                (Integer.parseInt(note.contentKey)+1));
    }
    public void updateNote(String loginName,Notes note){
        db.execSQL("update user_"+loginName+" set title = '"+note.title+"',date = '"
        +note.date+"' where _id = " + (Integer.parseInt(note.contentKey)+1));
    }

    //判断登录密码是否正确
    public boolean isPasswordRight(final String user,final String password){
        Cursor cursor = db.rawQuery("select password from user where user_name = ?",
                new String[]{user}) ;
        if(cursor.moveToFirst()){
            if(cursor.getString(0).equals(password)) return true;
        }
        return false;
    }
    //注册成功之后会调用此函数以user命名.创建每个用户的记事本项目表.
    //由于并不能用alter 语句修改列名,暂时用file_path代表 title
    public void createSingleTable(final String user){
        String CREATE_ITEMS_TABLE_SQL = "create table user_"+user+"(_id integer primary" +
                " key autoincrement,date,title)";
        db.execSQL(CREATE_ITEMS_TABLE_SQL);
    }

    //注册成功后会更新数据库中的user表
    public void updateUser(final String user,final String password){
        db.execSQL("insert into user values(null,?,?)",new String[]{user,password});
    }
    //根据loginName 来得到该用户所有的记事条目信息
    public List<Notes> getNotes(String loginName){
        ArrayList<Notes> notes = new ArrayList<Notes>();
        Cursor cursor = db.rawQuery("select * from user_"+loginName,null);
        while(cursor.moveToNext()){
            Notes note = new Notes();
            //表中第一列是主键id 也是SharedPreferences 里取记事内容的key
            //表中第二列是 日期
            //表中第三列是 标题名称
            note.title = cursor.getString(2);
            note.date = cursor.getString(1);
            note.contentKey = cursor.getInt(0)-1+"";
            notes.add(note);
        }
        return notes;
    }
}
