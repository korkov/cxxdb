#ifndef CXXDB_HPP_1350390166
#define CXXDB_HPP_1350390166

#include <map>
#include <boost/typeof/typeof.hpp>
#include <boost/mpl/find.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/tuple.hpp>

namespace cxxdb
{
  namespace cxxdb_details
  {
    template <typename Column, typename Tuple_ptr>
    struct db_column
    {
      typedef std::multimap<typename Column::type, Tuple_ptr> map_type;
      map_type map;

      typedef typename map_type::const_iterator const_iterator;

      map_type& get_map()
      {
        return map;
      }
    };

    template <size_t Num, typename Output, typename Input>
    struct convert_tuple_helper
    {
      static void copy(Output& out, Input const& in)
      {
        boost::fusion::at_c<Num - 1>(out).value = boost::fusion::at_c<Num - 1>(in);
        convert_tuple_helper<Num - 1, Output, Input>::copy(out, in);
      }
    };

    template <typename Output, typename Input>
    struct convert_tuple_helper<0, Output, Input>
    {
      static void copy(Output& out, Input const& in)
      {
      }
    };

    template <typename Output, typename Input>
    Output convert_tuple(Input const& in)
    {
      Output ret;
      convert_tuple_helper<boost::fusion::tuple_size<Output>::value, Output, Input>::copy(ret, in);
      return ret;
    }


    template <typename Tuple, size_t num = boost::fusion::tuple_size<Tuple>::value>
    struct inherit_tuple_elements
      : boost::fusion::tuple_element<num, Tuple>::type
      , inherit_tuple_elements<Tuple, num - 1>
    {
    };

    template <typename Tuple>
    struct inherit_tuple_elements<Tuple, 0>
      : boost::fusion::tuple_element<0, Tuple>::type
    {
    };

    template <typename Tuple, typename InternalTuple, typename T>
    struct get_db_column
    {
      typedef db_column<T, boost::shared_ptr<InternalTuple> > type;
    };

    template <typename Tuple>
    struct db
    {
      template <typename T>
      struct get_internal_type
      {
        typedef typename T::type type;
      };
      typedef typename boost::mpl::transform<Tuple, get_internal_type<boost::mpl::_> >::type data_t;

      struct columns_type
        : inherit_tuple_elements<
        typename boost::mpl::transform<Tuple, get_db_column<Tuple, data_t, boost::mpl::_> >::type
        >
      {
      } columns;


    typedef boost::shared_ptr<data_t> tuple_ptr;

    struct inserter
    {
      inserter(db& d, tuple_ptr p): m_db(d), m_ptr(p) {}

      template<typename T>
      void operator()(T& t) const
      {
        typedef db_column<T, tuple_ptr> db_column_t;

        m_db.columns.db_column_t::get_map().insert(std::make_pair(t.value, m_ptr));
      }

      db& m_db;
      tuple_ptr& m_ptr;
    };

    void insert(data_t line)
    {
      tuple_ptr ptr(new data_t(line));

      Tuple t = convert_tuple<Tuple>(line);

      boost::fusion::for_each(t, inserter(*this, ptr));
    }

    template <typename T>
    std::pair<
      typename db_column<T, tuple_ptr>::const_iterator,
      typename db_column<T, tuple_ptr>::const_iterator
      > select(typename T::type const& value)
      {
        return columns.db_column<T, tuple_ptr>::get_map().equal_range(value);
      }

      template <typename Column, typename Iter>
      static typename Column::type get_field(Iter& iter)
      {
        BOOST_AUTO(line, *(iter->second));
        Tuple t = convert_tuple<Tuple>(line);
        return (*boost::fusion::find<Column>(t)).value;
      }

      template <typename Iter>
      static data_t const& get(Iter& iter)
      {
        return *(iter->second);
      }
    };

    template <typename T>
    struct field
    {
      typedef T type;
      type value;
    };

  }

  using cxxdb_details::field;
  using cxxdb_details::db;

  using boost::fusion::tuple;
}

#endif // CXXDB_HPP_1350390166
