/***************************************************************************
 *            dataBaseSqlConstructor.cc
 *
 *  Mon Jul 18 2005
 *  Copyright  2005  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#include <libdodo/dataBaseSqlConstructor.h>
#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/toolsString.h>
#include <libdodo/dataBaseConnector.h>

using namespace dodo::data::base::sql;

const dodo::string constructor::statements[] = {
    "=",
    "='",
    "'",
    ",",
    " ( ",
    " ) ",
    "select ",
    " from ",
    "*",
    ":",
    "',",
    "),",
    "insert ",
    " into ",
    " values ",
    ") select ",
    "update ",
    " set ",
    "delete ",
    " where ",
    " limit ",
    " offset ",
    " order by ",
    " inner join ",
    " outer join ",
    " left join ",
    " right join ",
    " on ",
    "NULL",
};

//-------------------------------------------------------------------

condition::condition() : _limit(-1),
                         _offset(-1)
{
}

//-------------------------------------------------------------------

condition::condition(const dodo::string &table,
                     const dodo::string &statement) : _limit(-1),
                                                    _offset(-1),
                                                    _table(table),
                                                    _statement(statement)
{
}

//-------------------------------------------------------------------

condition::condition(const dodo::string &table,
                     const dodoStringArray &fields,
                     const dodo::string &statement) : _limit(-1),
                                                    _offset(-1),
                                                    _table(table),
                                                    _statement(statement),
                                                    _fields(fields)
{
}

//-------------------------------------------------------------------

const condition &
condition::limit(long a_limit) const
{
    _limit = a_limit;

    return *this;
}

//-------------------------------------------------------------------

const condition &
condition::offset(long a_offset) const
{
    _offset = a_offset;

    return *this;
}

//-------------------------------------------------------------------

const condition &
condition::order(const dodo::string &field,
                 short direction) const
{
    dodo::string dir[] = {
        " ASC",
        " DESC",
    };

    switch (direction) {
        case ORDER_DIRECTION_ASC:
        case ORDER_DIRECTION_DESC:
            if (!_orderby.empty())
                _orderby += dodo::string(", ");
            _orderby += dodo::string(field);
            _orderby += dodo::string(dir[direction]);
    }

    return *this;
}

//-------------------------------------------------------------------

const condition &
condition::join(const dodo::string &table,
                const dodo::string &condition,
                short type) const
{
    __join__ j;

    j.type = type;
    j.table = table;
    j.condition = condition;

    _join.push_back(j);

    return *this;
}

//-------------------------------------------------------------------

condition::~condition()
{
}

//-------------------------------------------------------------------

rows::rows()
{
}

//-------------------------------------------------------------------

rows::rows(const dodoArray<dodoStringArray> &values,
           const dodoStringArray &fields) : fields(fields),
                                           values(values)
{
}

//-------------------------------------------------------------------

rows::rows(const dodoStringArray &a_values,
           const dodoStringArray &fields) : fields(fields)
{
    values.push_back(a_values);
}

//-------------------------------------------------------------------

rows::rows(const dodoArray<dodoStringMap> &map)
{
    dodoArray<dodoStringMap>::const_iterator i = map.begin(), j = map.end();

    if (i != j) {
        dodoStringMap::const_iterator o = i->begin(), p = i->end();

        if (o != p) {
            for (;o!=p;++o)
                fields.push_back(o->first);

            dodoStringArray array;
#ifndef USE_DEQUE
            array.reserve(i->size());
#endif

            for (;i!=j;++i) {
                o = i->begin();
                p = i->end();

                for (;o!=p;++o)
                    array.push_back(o->second);
                values.push_back(array);

                array.clear();
            }
        }
    }
}

//-------------------------------------------------------------------

rows::rows(const dodoStringMap &map)
{
    dodoStringMap::const_iterator o = map.begin(), p = map.end();
    if (o!=p) {
        for (;o!=p;++o)
            fields.push_back(o->first);

        o = map.begin();

        dodoStringArray array;
#ifndef USE_DEQUE
        array.reserve(map.size());
#endif

        for (;o!=p;++o)
            array.push_back(o->second);
        values.push_back(array);
    }
}

//-------------------------------------------------------------------

rows::~rows()
{
}

//-------------------------------------------------------------------

query::query()
{
}

//-------------------------------------------------------------------

query::query(const dodo::string &sql) : sql(sql)
{
}

//-------------------------------------------------------------------

query::~query()
{
}

//-------------------------------------------------------------------

#ifndef DATABASE_WO_XEXEC
constructor::__collected_data__::__collected_data__(xexec *a_executor,
                                                    short execObject) : xexec::__collected_data__(a_executor, execObject),
                                                                        type(REQUEST_NONE),
                                                                        query(NULL)
{
}
#else
constructor::__collected_data__::__collected_data__() : type(REQUEST_NONE),
                                                        query(NULL)
{
}
#endif

//-------------------------------------------------------------------

void
constructor::__collected_data__::clear()
{
    query = NULL;
    type = REQUEST_NONE;
    condition._orderby.clear();
    condition._limit = -1;
    condition._offset = -1;
    condition._table.clear();
    condition._statement.clear();
    condition._fields.clear();
    condition._join.clear();
    rows.fields.clear();
    rows.values.clear();
}

//-------------------------------------------------------------------

constructor::constructor()
#ifndef DATABASE_WO_XEXEC
    : collectedData(this, xexec::OBJECT_XEXEC)
#endif
{
}

//-------------------------------------------------------------------

constructor::~constructor()
{
}

//-------------------------------------------------------------------

void
constructor::select(const dodo::data::base::condition &condition)
{
    collectedData.condition = *dynamic_cast<const sql::condition *>(&condition);

    collectedData.type = REQUEST_SELECT;
}

//-------------------------------------------------------------------

void
constructor::insert(const dodo::data::base::rows      &rows,
                    const dodo::data::base::condition &condition)
{
    collectedData.condition = *dynamic_cast<const sql::condition *>(&condition);
    collectedData.rows = *dynamic_cast<const sql::rows *>(&rows);

    collectedData.type = REQUEST_INSERT;
}

//-------------------------------------------------------------------

void
constructor::update(const dodo::data::base::rows      &rows,
                    const dodo::data::base::condition &condition)
{
    collectedData.condition = *dynamic_cast<const sql::condition *>(&condition);
    collectedData.rows = *dynamic_cast<const sql::rows *>(&rows);

    collectedData.type = REQUEST_UPDATE;
}

//-------------------------------------------------------------------

void
constructor::remove(const dodo::data::base::condition &condition)
{
    collectedData.condition = *dynamic_cast<const sql::condition *>(&condition);

    collectedData.type = REQUEST_REMOVE;
}

//-------------------------------------------------------------------

void
constructor::setFieldType(const dodo::string &table,
                          const dodo::string &field,
                          short            type)
{
    fieldTypes[table][field] = type;
}

//-------------------------------------------------------------------

dodo::string
constructor::select()
{
    dodo::string request;

    if (collectedData.condition._table.size() > 0) {
        request = statements[STATEMENT_SELECT];
        if (collectedData.condition._fields.size() == 0)
            request += dodo::string(statements[STATEMENT_STAR]);
        else
            request += dodo::string(tools::misc::join(collectedData.condition._fields, statements[STATEMENT_COMA]));
        request += dodo::string(statements[STATEMENT_FROM]);
        request += dodo::string(collectedData.condition._table);
    } else {
        request = statements[STATEMENT_SELECT];
        if (collectedData.condition._fields.size() == 0)
            request += dodo::string(statements[STATEMENT_STAR]);
        else
            request += dodo::string(tools::misc::join(collectedData.condition._fields, statements[STATEMENT_COMA]));
    }

    if (collectedData.condition._statement.size() > 0) {
        request += dodo::string(statements[STATEMENT_WHERE]);
        request += dodo::string(collectedData.condition._statement);
    }

    if (collectedData.condition._orderby.size() > 0) {
        request += dodo::string(statements[STATEMENT_ORDERBY]);
        request += dodo::string(collectedData.condition._orderby);
    }

    if (collectedData.condition._limit != -1) {
        request += dodo::string(statements[STATEMENT_LIMIT]);
        request += dodo::string(tools::string::lToString(collectedData.condition._limit));

        if (collectedData.condition._offset != -1) {
            request += dodo::string(statements[STATEMENT_OFFSET]);
            request += dodo::string(tools::string::lToString(collectedData.condition._offset));
        }
    }

    if (collectedData.condition._join.size() > 0) {
        dodoArray<condition::__join__>::iterator i = collectedData.condition._join.begin(), j = collectedData.condition._join.end();
        for (;i!=j;++i) {
            switch (i->type) {
                case condition::JOIN_LEFT:
                    request += dodo::string(statements[STATEMENT_JOINLEFT]);

                    break;

                case condition::JOIN_RIGHT:
                    request += dodo::string(statements[STATEMENT_JOINRIGHT]);

                    break;

                case condition::JOIN_INNER:
                    request += dodo::string(statements[STATEMENT_JOININNER]);

                    break;

                case condition::JOIN_OUTER:
                    request += dodo::string(statements[STATEMENT_JOINOUTER]);

                    break;
            }
            request += dodo::string(i->table);
            if (!i->condition.empty()) {
                request += dodo::string(statements[STATEMENT_ON]);
                request += dodo::string(i->condition);
            }
        }
    }

    return request;
}

//-------------------------------------------------------------------

dodo::string
constructor::insert()
{
    dodo::string request = statements[STATEMENT_INSERT];

    request += dodo::string(statements[STATEMENT_INTO]);
    request += dodo::string(collectedData.condition._table);
    if (collectedData.rows.fields.size() != 0) {
        request += dodo::string(statements[STATEMENT_LEFTBRACKET]);
        request += dodo::string(tools::misc::join(collectedData.rows.fields, statements[STATEMENT_COMA]));
        request += dodo::string(statements[STATEMENT_RIGHTBRACKET]);
    }
    request += dodo::string(statements[STATEMENT_VALUES]);

    dodoArray<dodoStringArray>::iterator k(collectedData.rows.values.begin()), l(collectedData.rows.values.end());
    if (k != l) {
        dodoMap<dodo::string, dodoMap<dodo::string, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.condition._table);
        if (types != fieldTypes.end()) {
            dodoMap<dodo::string, short, dodoMapICaseStringCompare>::iterator type;
            dodoMap<dodo::string, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

            dodoStringArray::iterator t;

            --l;
            for (; k != l; ++k) {
                request += dodo::string(statements[STATEMENT_LEFTBRACKET]);

                t = collectedData.rows.fields.begin();

                dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
                for (; i != j; ++i, ++t) {
                    type = types->second.find(*t);
                    if (type != typesEnd) {
                        if (type->second == FIELD_TEXT || type->second == FIELD_BINARY)
                            request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
                        else
                            request += dodo::string(*i + statements[STATEMENT_COMA]);
                    } else
                        request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
                }
                type = types->second.find(*t);
                if (type != typesEnd) {
                    if (type->second == FIELD_TEXT || type->second == FIELD_BINARY)
                        request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);
                    else
                        request += dodo::string(*i);
                } else
                    request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);

                request += dodo::string(statements[STATEMENT_RIGHTBRACKETCOMA]);
            }
            request += dodo::string(statements[STATEMENT_LEFTBRACKET]);

            t = collectedData.rows.fields.begin();

            dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
            for (; i != j; ++i, ++t) {
                type = types->second.find(*t);
                if (type != typesEnd) {
                    if (type->second == FIELD_TEXT || type->second == FIELD_BINARY)
                        request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
                    else
                        request += dodo::string(*i + statements[STATEMENT_COMA]);
                } else
                    request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
            }
            type = types->second.find(*t);
            if (type != typesEnd) {
                if (type->second == FIELD_TEXT || type->second == FIELD_BINARY)
                    request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);
                else
                    request += dodo::string(*i);
            } else
                request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);

            request += dodo::string(statements[STATEMENT_RIGHTBRACKET]);
        } else {
            --l;
            for (; k != l; ++k) {
                request += dodo::string(statements[STATEMENT_LEFTBRACKET]);
                request += dodo::string(joinFields(*k, statements[STATEMENT_COMA], statements[STATEMENT_APOSTROPHE]));
                request += dodo::string(statements[STATEMENT_RIGHTBRACKETCOMA]);
            }
            request += dodo::string(statements[STATEMENT_LEFTBRACKET]);
            request += dodo::string(joinFields(*k, statements[STATEMENT_COMA], statements[STATEMENT_APOSTROPHE]));
            request += dodo::string(statements[STATEMENT_RIGHTBRACKET]);
        }
    }

    return request;
}

//-------------------------------------------------------------------

dodo::string
constructor::update()
{
    dodo::string request = statements[STATEMENT_UPDATE];

    request += dodo::string(collectedData.condition._table);
    request += dodo::string(statements[STATEMENT_SET]);

    dodoArray<dodoStringArray>::iterator v = collectedData.rows.values.begin();
    if (v != collectedData.rows.values.end()) {
        dodoStringArray::const_iterator i(collectedData.rows.fields.begin()), j(v->begin());
        if (i != j) {
            unsigned int fn(collectedData.rows.fields.size()), fv(v->size());

            unsigned int o(fn <= fv ? fn : fv);

            dodoMap<dodo::string, dodoMap<dodo::string, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.condition._table);
            if (types != fieldTypes.end()) {
                dodoMap<dodo::string, short, dodoMapICaseStringCompare>::iterator type;
                dodoMap<dodo::string, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

                --o;
                for (unsigned int k(0); k < o; ++i, ++k, ++j) {
                    request += dodo::string(*i);

                    type = types->second.find(*i);
                    if (type != typesEnd) {
                        if (type->second == FIELD_TEXT || type->second == FIELD_BINARY) {
                            request += dodo::string(statements[STATEMENT_EQUALAPOSTROPHE]);
                            request += dodo::string(escapeFields(*j));
                            request += dodo::string(statements[STATEMENT_APOSTROPHECOMA]);
                        } else {
                            request += dodo::string(statements[STATEMENT_EQUAL]);
                            request += dodo::string(*j);
                            request += dodo::string(statements[STATEMENT_COMA]);
                        }
                    } else {
                        request += dodo::string(statements[STATEMENT_EQUALAPOSTROPHE]);
                        request += dodo::string(escapeFields(*j));
                        request += dodo::string(statements[STATEMENT_APOSTROPHECOMA]);
                    }
                }
                request += dodo::string(*i);

                type = types->second.find(*i);
                if (type != typesEnd) {
                    if (type->second == FIELD_TEXT || type->second == FIELD_BINARY) {
                        request += dodo::string(statements[STATEMENT_EQUALAPOSTROPHE]);
                        request += dodo::string(escapeFields(*j));
                        request += dodo::string(statements[STATEMENT_APOSTROPHE]);
                    } else {
                        request += dodo::string(statements[STATEMENT_EQUAL]);
                        request += dodo::string(*j);
                    }
                } else {
                    request += dodo::string(statements[STATEMENT_EQUALAPOSTROPHE]);
                    request += dodo::string(escapeFields(*j));
                    request += dodo::string(statements[STATEMENT_APOSTROPHE]);
                }
            } else {
                --o;
                for (unsigned int k(0); k < o; ++i, ++k, ++j) {
                    request += dodo::string(*i);
                    request += dodo::string(statements[STATEMENT_EQUALAPOSTROPHE]);
                    request += dodo::string(escapeFields(*j));
                    request += dodo::string(statements[STATEMENT_APOSTROPHECOMA]);
                }
                request += dodo::string(*i);
                request += dodo::string(statements[STATEMENT_EQUALAPOSTROPHE]);
                request += dodo::string(escapeFields(*j));
                request += dodo::string(statements[STATEMENT_APOSTROPHE]);
            }
        }
    }

    if (collectedData.condition._statement.size() > 0) {
        request += dodo::string(statements[STATEMENT_WHERE]);
        request += dodo::string(collectedData.condition._statement);
    }

    if (collectedData.condition._orderby.size() > 0) {
        request += dodo::string(statements[STATEMENT_ORDERBY]);
        request += dodo::string(collectedData.condition._orderby);
    }

    if (collectedData.condition._limit != -1) {
        request += dodo::string(statements[STATEMENT_LIMIT]);
        request += dodo::string(tools::string::lToString(collectedData.condition._limit));
    }

    return request;
}

//-------------------------------------------------------------------

dodo::string
constructor::remove()
{
    dodo::string request = statements[STATEMENT_DELETE];

    request += dodo::string(statements[STATEMENT_FROM]);
    request += dodo::string(collectedData.condition._table);

    if (collectedData.condition._statement.size() > 0) {
        request += dodo::string(statements[STATEMENT_WHERE]);
        request += dodo::string(collectedData.condition._statement);
    }

    if (collectedData.condition._orderby.size() > 0) {
        request += dodo::string(statements[STATEMENT_ORDERBY]);
        request += dodo::string(collectedData.condition._orderby);
    }

    if (collectedData.condition._limit != -1) {
        request += dodo::string(statements[STATEMENT_LIMIT]);
        request += dodo::string(tools::string::lToString(collectedData.condition._limit));
    }

    return request;
}

//-------------------------------------------------------------------

dodo::string
constructor::construct()
{
    switch (collectedData.type) {
        case REQUEST_SELECT:
            return select();

        case REQUEST_INSERT:
            return insert();

        case REQUEST_UPDATE:
            return update();

        case REQUEST_REMOVE:
            return remove();
    }

    return __dodostring__;
}

//-------------------------------------------------------------------

dodo::string
constructor::escapeFields(const dodo::string &data)
{
    unsigned long size = data.size();
    dodo::string temp('\0', size+size/4);

    temp.clear();

    for (unsigned long i = 0; i < size; ++i) {
        if (data[i] == '\\' || data == '\'')
            temp += '\\';
        temp += data[i];
    }

    return temp;
}

//-------------------------------------------------------------------

dodo::string
constructor::joinFields(const dodoStringArray &fields,
                        const dodo::string      &separator,
                        const dodo::string      &frame,
                        int                   limit)
{
    int k(0);

    dodo::string temp, fs(frame + separator);
    dodoStringArray::const_iterator i(fields.begin()), j(fields.end());
    if (i != j) {
        --j;
        for (; i != j; ++i) {
            if (limit != -1) {
                if (k > limit)
                    return temp;
                ++k;
            }
            temp += dodo::string(frame);
            temp += dodo::string(escapeFields(*i));
            temp += dodo::string(fs);
        }
        temp += dodo::string(frame);
        temp += dodo::string(escapeFields(*i));
        temp += dodo::string(frame);
    }

    return temp;
}

//-------------------------------------------------------------------
