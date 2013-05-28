cxxdb
=====

Simple C++ memory database (multi index storage) with not simple code.

### Example:

Declare fields of table:
```c++
struct id: cxxdb::field<size_t> {};
struct name: cxxdb::field<std::string> {};
struct email: cxxdb::field<std::string> {};
struct age: cxxdb::field<size_t> {};
```
Now we can declare table:

```c++
typedef cxxdb::db< cxxdb::tuple<id, name, email, age> > my_db;
```

insert:

```c++
  my_db db;

  db.insert(my_db::data_t(1, "alexander", "mail_1@gmail.com", 26));
  db.insert(my_db::data_t(2, "alexander", "mail_2@gmail.com", 30));
```

select:

```c++
  BOOST_AUTO(range, db.select<name>("alexander"));

  for(
      BOOST_AUTO(iter, range.first);
      iter != range.second;
      ++iter)
    {
      // print some fileds
      LOG(
          " name: " << my_db::get_field<name>(iter) <<
          " email: " << my_db::get_field<email>(iter) <<
          " | " <<
          // print whole line
          my_db::get(iter)
          );
    }
```


--
Alexander Korkov
e-mail: korkov@yandex.ru
