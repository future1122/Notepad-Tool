package com.example.damon.notepad;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by damon on 17-9-6.
 * 程序的主Activity
 * bug: 批量删除后自动返回 list界面,但是 菜单的checked 属性改变不了.
 */

public class MainActivity extends AppCompatActivity {
    final static int REQUEST_CODE = 0;
    final static int UPDATE_LISTVIEW = 0x123;
    final static int UPDATE_DELETE_VIEW = 0x111;
    final static int NEW_NOTE = 0;
    final static int UPDATE_NOTE = 1;
    final static int LOGIN_STATE_IN = 0;
    final static int LOGIN_STATE_OUT = 1;
    final static int SEARCH_DATE = 0;
    final static int SEARCH_TITLE = 1;
    final static String LOGIN_STATE = "login state";
    final static String LOGIN_MESSAGE = "login";
    final static String LOGIN_NAME = "name";
    final static String PASSWORD = "password";
    final static String START_STATE = "state";
    final static String TITLE = "title";
    final static String CONTENT = "content";
    final static String DATE = "date";

    int searchTypes = SEARCH_DATE;
    //用户名
    String loginName;
    //该用户的记事条目在数据库中的表名
    List<Notes> notes;
    List<Notes> searchNotes;
    //用来更新批量删除的listview.
    Map<String,Boolean> checkedMap;

    //数据库
    MyDatabaseHelper databaseHelper;
    //用来保存记事的内容
    SharedPreferences preferences;
    //用来修改记事内容
    SharedPreferences.Editor editor;
    //显示记事条目
    ListView showNotes;
    CheckBox selectAll;  //全选
    TextView selectHint; //提示信息
    Button delete;       //删除
    EditText searchEdit;
    Spinner searchType;
    Button cancelSearch;
    //一些提示信息.
    Button hint;
    //更新listview
    Handler handler ;
    //是否显示搜索列表
    boolean isSearching = false;
    boolean isDeletingByQuery=false;
    //用来判断是否已经搜索过.
    String lastKey;
    //当前正在编辑的note编号.
    int current;
    //保存listview滑动的位置
    int index = 0;
    int top = 0;
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //初始化组件
        initComp();
        //如果保存了用户的登录信息,直接登录.
        preferences = getSharedPreferences(LOGIN_MESSAGE,Context.MODE_WORLD_READABLE);
        editor = preferences.edit();
        String savedPassword = preferences.getString(PASSWORD,"");
        int loginState = preferences.getInt(LOGIN_STATE,LOGIN_STATE_OUT);
        //如果没有保存密码就重新登录
        if(loginState == LOGIN_STATE_OUT || savedPassword == null || savedPassword.equals("")){
            editor.putInt(LOGIN_STATE,LOGIN_STATE_OUT);
            editor.commit();
            logout();
        }
        else{
            //获取登录账号信息.
            loginName = preferences.getString(LOGIN_NAME,"");
            //
            Toast.makeText(this, "账号 "+loginName+" 登录成功", Toast.LENGTH_SHORT).show();
            initWithLoginName();
        }



    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = new MenuInflater(this);
        inflater.inflate(R.menu.menu,menu);
        return super.onCreateOptionsMenu(menu);
    }
    //菜单点击事件.
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()){
            //退出登录
            case R.id.back_to_login:
                preferences = getSharedPreferences(LOGIN_MESSAGE,Context.MODE_WORLD_READABLE);
                editor = preferences.edit();
                editor.putInt(LOGIN_STATE,LOGIN_STATE_OUT);
                editor.commit();
                logout();
                break;
            //批量删除
            case R.id.delete:
                if(item.isChecked()) {
                    handler.sendEmptyMessage(UPDATE_LISTVIEW);
                    item.setChecked(false);
                    isDeletingByQuery = false;
                    hideDelete();
                }
                else {
                    deleteByQuery();
                    item.setChecked(true);
                    isDeletingByQuery = true;
                    showDelete();
                }
                break;
            //新建条目.
            case R.id.new_note:
                 createNote();
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    //批量删除
    private void deleteByQuery() {
        checkedMap = new HashMap<String, Boolean>();
        handler.sendEmptyMessage(UPDATE_DELETE_VIEW);
    }

    //创建新记事条目
    private void createNote() {
        Intent intent = new Intent(MainActivity.this,EditNote.class);
        //表示新建记事条目
        intent.putExtra(START_STATE,NEW_NOTE);
        intent.putExtra(TITLE,"");
        intent.putExtra(CONTENT,"");
        startActivityForResult(intent,REQUEST_CODE);
    }

    //登出账号.
    private void logout() {
        Intent intent = new Intent(MainActivity.this,LoginActivity.class);
        startActivityForResult(intent,REQUEST_CODE);
    }
    //初始化一些基础组件
    private void initComp() {

        checkedMap = new HashMap<String, Boolean>();
        notes = new ArrayList<Notes>();
        searchNotes = new ArrayList<Notes>();
        selectAll = (CheckBox) findViewById(R.id.select_all);
        selectHint = (TextView) findViewById(R.id.select_hint);
        delete = (Button) findViewById(R.id.confirm);
        //-----------------------

        //全选
        selectAll.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                List<Notes> temp = null;
                if(isSearching)temp = searchNotes;
                else temp = notes;
                if(selectAll.isChecked()){

                    for(int j = 0;j<temp.size();j++){
                        checkedMap.put(temp.get(j).contentKey,true);
                    }
                }
                else{
                    for(int j = 0;j<temp.size();j++){
                        checkedMap.remove(temp.get(j).contentKey);
                    }
                }
                handler.sendEmptyMessage(UPDATE_DELETE_VIEW);
            }
        });

        //删除按钮
        delete.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(checkedMap.size()>0){
                    new AlertDialog.Builder(MainActivity.this)
                            .setTitle("提示")
                            .setMessage("确认要删除这"+checkedMap.size()+"个选项吗?")
                            .setPositiveButton("确认", new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialogInterface, int i) {
                                    deleteSelectedItems();
                                }
                            })
                            .setNegativeButton("取消",null)
                            .show();
                }
                else {
                    new AlertDialog.Builder(MainActivity.this)
                            .setTitle("提示")
                            .setMessage("没有要删除的项目")
                            .setPositiveButton("确认",null)
                            .show();
                }
            }
        });
        //-----------------------
        handler = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                List<Notes> temp = null;
                if(isSearching)temp = searchNotes;
                else temp = notes;
                if(isSelectAll(temp)){
                    selectAll.setChecked(true);
                }
                else selectAll.setChecked(false);

                selectHint.setText("一共选了"+checkedMap.size()+"个选项");
                index = showNotes.getFirstVisiblePosition();
                View v = showNotes.getChildAt(0);
                top =  (v == null)?0:v.getTop();
                switch (msg.what){
                    case UPDATE_LISTVIEW:
                        //为listview设置点击时间.可以点击来修改记事内容.
                        showNotes.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                            @Override
                            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                                //打开编辑页面.
                                current = i;
                                Notes note = notes.get(i);
                                Intent intent  = new Intent(MainActivity.this,EditNote.class);
                                intent.putExtra(START_STATE,UPDATE_NOTE);
                                intent.putExtra(TITLE,note.title);
                                intent.putExtra(CONTENT,note.content);
                                startActivityForResult(intent,REQUEST_CODE);
                            }
                        });
                        //更新界面
                        if(temp.size() == 0) hint.setText("没有记事项目");
                        else hint.setText("一共有"+temp.size()+"项");
                        if(temp.size() == 0&& isSearching) hint.setText("无匹配项目");
                        ShowNotesBaseAdapter adapter =
                                new ShowNotesBaseAdapter(temp,MainActivity.this);
                        showNotes.setAdapter(adapter);
                        break;
                    case UPDATE_DELETE_VIEW:
                        showNotes.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                            @Override
                            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                                List<Notes> temp = null;
                                if(isSearching)temp = searchNotes;
                                else temp = notes;
                                if(checkedMap.containsKey(temp.get(i).contentKey))
                                    checkedMap.remove(temp.get(i).contentKey);
                                else checkedMap.put(temp.get(i).contentKey,true);
                                handler.sendEmptyMessage(UPDATE_DELETE_VIEW);
                            }
                        });
                        if(temp.size() == 0) hint.setText("没有记事项目");
                        else hint.setText("一共有"+temp.size()+"项");
                        if(temp.size() == 0&& isSearching) hint.setText("无匹配项目");
                        DeleteByQueryBaseAdapter adapter2 = new
                                DeleteByQueryBaseAdapter(MainActivity.this,temp);
                        showNotes.setAdapter(adapter2);
                        break;
                }
                showNotes.setSelectionFromTop(index,top);
            }
        };
        showNotes = (ListView) findViewById(R.id.show_notes);
        searchEdit = (EditText) findViewById(R.id.search_edit);
        searchEdit.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View view, boolean b) {
                if(b) isSearching = true;
                else isSearching = false;
            }
        });
        searchType = (Spinner) findViewById(R.id.search_type);
        searchType.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                if(i == 0) {
                    searchTypes = SEARCH_DATE;
                    searchEdit.setHint("yyyy-MM-dd或数字.");
                }
                else {
                    searchTypes = SEARCH_TITLE;
                    searchEdit.setHint("填写部分标题.");
                }
                search();
                if(isDeletingByQuery)handler.sendEmptyMessage(UPDATE_DELETE_VIEW);
                else handler.sendEmptyMessage(UPDATE_LISTVIEW);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
        cancelSearch = (Button) findViewById(R.id.cancel_search_btn);
        cancelSearch.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                RelativeLayout ll = (RelativeLayout) findViewById(R.id.search_layout);
                ll.setFocusable(true);
                ll.setFocusableInTouchMode(true);
                ll.requestFocus();
                searchEdit.setText("");
                InputMethodManager mm = (InputMethodManager)
                        getSystemService(Context.INPUT_METHOD_SERVICE);

                mm.hideSoftInputFromWindow(searchEdit.getApplicationWindowToken(),0);
                if(isDeletingByQuery)handler.sendEmptyMessage(UPDATE_DELETE_VIEW);
                else handler.sendEmptyMessage(UPDATE_LISTVIEW);
            }
        });
        hint = (Button) findViewById(R.id.hint);
        databaseHelper = new MyDatabaseHelper(this,"mydata.db3",1);
        //搜索线程
        new Thread(){
            @Override
            public void run() {
                super.run();
                while (true){
                    if(searchEdit.isFocused()){
                        String key = searchEdit.getText().toString();
                        if(!key.equals(lastKey)){
                            search();
                            if(isDeletingByQuery)handler.sendEmptyMessage(UPDATE_DELETE_VIEW);
                            else handler.sendEmptyMessage(UPDATE_LISTVIEW);
                        }

                    }
                    try {
                        sleep(500);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }.start();
    }

    private boolean isSelectAll(List<Notes> temp) {
        for(int i=0;i<temp.size();i++){
            Notes note = temp.get(i);
            if(!checkedMap.containsKey(note.contentKey))
                return false;
        }
        return true;
    }

    //查询
    private void search() {
        String[] dates = null;
        searchNotes = new ArrayList<Notes>();
        //用户输入的搜索关键字
        String key = searchEdit.getText().toString();
        lastKey = key;
        if(key == null || key.equals("")) {
            searchNotes = notes;
        }
        else{
            switch (searchTypes){
                case SEARCH_DATE:
                    for(int i = 0;i<notes.size();i++){
                        Notes note = notes.get(i);
                        if(isSubString(note.date.substring(0,10),key)) searchNotes.add(note);
                    }
                    break;
                case SEARCH_TITLE:
                    for(int i = 0;i<notes.size();i++){
                        Notes note = notes.get(i);
                        if(isSubString(note.title,key)) searchNotes.add(note);
                    }
                    break;
            }
        }
    }

    //登录界面返回结果.账号是哪个.然后根据返回的结果初始化主界面.
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (requestCode){
            case REQUEST_CODE:
                switch (resultCode){

                    case LoginActivity.RESULT_CODE:
                        //获取登录账号信息.
                        loginName = data.getStringExtra("userName");
                        //
                        Toast.makeText(this, "账号 "+loginName+" 登录成功", Toast.LENGTH_SHORT).show();
                        initWithLoginName();
                        break;
                    //根据编辑页面返回的信息来更新数据以及界面.
                    case EditNote.RESULT_CODE:
                        switch (data.getIntExtra(START_STATE,20)){
                            //如果是新建的note
                            case NEW_NOTE:
                                Notes note = new Notes();
                                note.title=data.getStringExtra(TITLE);
                                note.content = data.getStringExtra(CONTENT);
                                note.date = data.getStringExtra(DATE);
                                note.contentKey = databaseHelper.addNote(loginName,note);
                                editor.putString(note.contentKey,note.content);
                                editor.commit();
                                notes.add(note);
                                if(isDeletingByQuery) handler.sendEmptyMessage(UPDATE_DELETE_VIEW);
                                else handler.sendEmptyMessage(UPDATE_LISTVIEW);
                                break;
                            //更新note
                            case UPDATE_NOTE:
                                Notes editingNote = notes.get(current);
                                editingNote.title = data.getStringExtra(TITLE);
                                editingNote.content = data.getStringExtra(CONTENT);
                                editingNote.date = data.getStringExtra(DATE);
                                databaseHelper.updateNote(loginName,editingNote);
                                editor.putString(editingNote.contentKey,editingNote.content);
                                editor.commit();
                                if(isDeletingByQuery) handler.sendEmptyMessage(UPDATE_DELETE_VIEW);
                                else handler.sendEmptyMessage(UPDATE_LISTVIEW);
                                break;
                        }
                        break;
                }
                break;
        }
    }

    //根据登录界面返回的信息来初始化 主界面.
    private void initWithLoginName() {
        //初始化后台信息        System.out.println("--------------------------------"+notes.size());

        preferences = getSharedPreferences(loginName, Context.MODE_WORLD_READABLE);
        editor = preferences.edit();
        notes = databaseHelper.getNotes(loginName);
        //初始化notes的内容.
        for(int i = 0;i < notes.size();i++){
            Notes note = notes.get(i);
            note.content = preferences.getString(note.contentKey,"没有获取到记事内容");
        }
        //初始化界面信息
        if(isDeletingByQuery) handler.sendEmptyMessage(UPDATE_DELETE_VIEW);
        else handler.sendEmptyMessage(UPDATE_LISTVIEW);

    }
    //出现批量操作界面
    private void showDelete(){
        RelativeLayout rl = (RelativeLayout) findViewById(R.id.operate_delete);
        RelativeLayout.LayoutParams lp = (RelativeLayout.LayoutParams) rl.getLayoutParams();
        lp.height = 100;
        rl.setLayoutParams(lp);
    }
    //隐藏批量操作界面.
    private void hideDelete(){
        RelativeLayout rl = (RelativeLayout) findViewById(R.id.operate_delete);
        RelativeLayout.LayoutParams lp = (RelativeLayout.LayoutParams) rl.getLayoutParams();
        lp.height = 0;
        rl.setLayoutParams(lp);
    }
    //批量删除
    private void deleteSelectedItems() {
        List<Notes> temp = null;
        if(isSearching)temp = searchNotes;
        else temp = notes;
        for(int i = notes.size()-1;i >-1;i--){
            if(checkedMap.containsKey(temp.get(i).contentKey)) deleteNote(notes.get(i));
        }
        checkedMap.clear();
        handler.sendEmptyMessage(UPDATE_DELETE_VIEW);
    }
    //判断sub 是否是s的子串
    private boolean isSubString(String s, String sub){
        for(int i=0;i<s.length()-sub.length()+1;i++){
            if(s.substring(i,i+sub.length()).equals(sub)) return true;
        }
        return false ;
    }
    //删除一个记事项目.
    public void deleteNote(final Notes note) {
        notes.remove(note);

        //删除后台信息
        databaseHelper.deleteNote(loginName,note);
        editor.remove(note.contentKey);
    }

    @Override
    protected void onPause() {
        InputMethodManager mm = (InputMethodManager)
                getSystemService(Context.INPUT_METHOD_SERVICE);

        mm.hideSoftInputFromWindow(searchEdit.getApplicationWindowToken(),0);
        super.onPause();
    }

    @Override
    protected void onStop() {
        InputMethodManager mm = (InputMethodManager)
                getSystemService(Context.INPUT_METHOD_SERVICE);

        mm.hideSoftInputFromWindow(searchEdit.getApplicationWindowToken(),0);
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        InputMethodManager mm = (InputMethodManager)
                getSystemService(Context.INPUT_METHOD_SERVICE);

        mm.hideSoftInputFromWindow(searchEdit.getApplicationWindowToken(),0);
        databaseHelper.db.close();
        super.onDestroy();
    }

}
