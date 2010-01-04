/***************************************************************************
 *            dataBaseSqlConstructor.cc
 *
 *  Mon Jul 18 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
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

const dodoString constructor::statements[] = {
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

condition::condition(const dodoString &table,
                     const dodoString &statement) : _limit(-1),
                                                    _offset(-1),
                                                    _table(table),
                                                    _statement(statement)
{
}

//-------------------------------------------------------------------

condition::condition(const dodoString &table,
                     const dodoStringArray &fields,
                     const dodoString &statement) : _limit(-1),
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
condition::order(const dodoString &field,
                 short direction) const
{
    dodoString dir[] = {
        " ASC",
        " DESC",
    };

    switch (direction) {
        case ORDER_DIRECTION_ASC:
        case ORDER_DIRECTION_DESC:
            if (!_orderby.empty())
                _orderby.append(", ");
            _orderby.append(field);
            _orderby.append(dir[direction]);
    }

    return *this;
}

//-------------------------------------------------------------------

const condition &
condition::join(const dodoString &table,
                const dodoString &condition,
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
            array.reserve(o->size());
#endif

            for (;i!=j;++i) {
                array.clear();

                o = i->begin();
                p = i->end();

                for (;o!=p;++o)
                    array.push_back(o->second);
                values.push_back(array);
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
        array.reserve(o->size());
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

query::query(const dodoString &sql) : sql(sql)
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
constructor::setFieldType(const dodoString &table,
                          const dodoString &field,
                          short            type)
{
    fieldTypes[table][field] = type;
}

//-------------------------------------------------------------------

dodoString
constructor::select()
{
    dodoString request;

    if (collectedData.condition._table.size() > 0) {
        request = statements[STATEMENT_SELECT];
        if (collectedData.condition._fields.size() == 0)
            request.append(statements[STATEMENT_STAR]);
        else
            request.append(tools::misc::join(collectedData.condition._fields, statements[STATEMENT_COMA]));
        request.append(statements[STATEMENT_FROM]);
        request.append(collectedData.condition._table);
    } else {
        request = statements[STATEMENT_SELECT];
        if (collectedData.condition._fields.size() == 0)
            request.append(statements[STATEMENT_STAR]);
        else
            request.append(tools::misc::join(collectedData.condition._fields, statements[STATEMENT_COMA]));
    }

    if (collectedData.condition._statement.size() > 0) {
        request.append(statements[STATEMENT_WHERE]);
        request.append(collectedData.condition._statement);
    }

    if (collectedData.condition._orderby.size() > 0) {
        request.append(statements[STATEMENT_ORDERBY]);
        request.append(collectedData.condition._orderby);
    }

    if (collectedData.condition._limit != -1) {
        request.append(statements[STATEMENT_LIMIT]);
        request.append(tools::string::lToString(collectedData.condition._limit));

        if (collectedData.condition._offset != -1) {
            request.append(statements[STATEMENT_OFFSET]);
            request.append(tools::string::lToString(collectedData.condition._offset));
        }
    }

    if (collectedData.condition._join.size() > 0) {
        dodoArray<condition::__join__>::iterator i = collectedData.condition._join.begin(), j = collectedData.condition._join.end();
        for (;i!=j;++i) {
            switch (i->type) {
                case condition::JOIN_LEFT:
                    request.append(statements[STATEMENT_JOINLEFT]);

                    break;

                case condition::JOIN_RIGHT:
                    request.append(statements[STATEMENT_JOINRIGHT]);

                    break;

                case condition::JOIN_INNER:
                    request.append(statements[STATEMENT_JOININNER]);

                    break;

                case condition::JOIN_OUTER:
                    request.append(statements[STATEMENT_JOINOUTER]);

                    break;
            }
            request.append(i->table);
            if (!i->condition.empty()) {
                request.append(statements[STATEMENT_ON]);
                request.append(i->condition);
            }
        }
    }

    return request;
}

//-------------------------------------------------------------------

dodoString
constructor::insert()
{
    dodoString request = statements[STATEMENT_INSERT];

    request.append(statements[STATEMENT_INTO]);
    request.append(collectedData.condition._table);
    if (collectedData.rows.fields.size() != 0) {
        request.append(statements[STATEMENT_LEFTBRACKET]);
        request.append(tools::misc::join(collectedData.rows.fields, statements[STATEMENT_COMA]));
        request.append(statements[STATEMENT_RIGHTBRACKET]);
    }
    request.append(statements[STATEMENT_VALUES]);

    dodoArray<dodoStringArray>::iterator k(collectedData.rows.values.begin()), l(collectedData.rows.values.end());
    if (k != l) {
        dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.condition._table);
        if (types != fieldTypes.end()) {
            dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator type;
            dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

            dodoStringArray::iterator t;

            --l;
            for (; k != l; ++k) {
                request.append(statements[STATEMENT_LEFTBRACKET]);

                t = collectedData.rows.fields.begin();

                dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
                for (; i != j; ++i, ++t) {
                    type = types->second.find(*t);
                    if (type != typesEnd) {
                        if (type->second == FIELD_TEXT || type->second == FIELD_BINARY)
                            request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
                        else
                            request.append(*i + statements[STATEMENT_COMA]);
                    } else
                        request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
                }
                type = types->second.find(*t);
                if (type != typesEnd) {
                    if (type->second == FIELD_TEXT || type->second == FIELD_BINARY)
                        request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);
                    else
                        request.append(*i);
                } else
                    request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);

                request.append(statements[STATEMENT_RIGHTBRACKETCOMA]);
            }
            request.append(statements[STATEMENT_LEFTBRACKET]);

            t = collectedData.rows.fields.begin();

            dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
            for (; i != j; ++i, ++t) {
                type = types->second.find(*t);
                if (type != typesEnd) {
                    if (type->second == FIELD_TEXT || type->second == FIELD_BINARY)
                        request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
                    else
                        request.append(*i + statements[STATEMENT_COMA]);
                } else
                    request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
            }
            type = types->second.find(*t);
            if (type != typesEnd) {
                if (type->second == FIELD_TEXT || type->second == FIELD_BINARY)
                    request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);
                else
                    request.append(*i);
            } else
                request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);

            request.append(statements[STATEMENT_RIGHTBRACKET]);
        } else {
            --l;
            for (; k != l; ++k) {
                request.append(statements[STATEMENT_LEFTBRACKET]);
                request.append(joinFields(*k, statements[STATEMENT_COMA], statements[STATEMENT_APOSTROPHE]));
                request.append(statements[STATEMENT_RIGHTBRACKETCOMA]);
            }
            request.append(statements[STATEMENT_LEFTBRACKET]);
            request.append(joinFields(*k, statements[STATEMENT_COMA], statements[STATEMENT_APOSTROPHE]));
            request.append(statements[STATEMENT_RIGHTBRACKET]);
        }
    }

    return request;
}

//-------------------------------------------------------------------

dodoString
constructor::update()
{
    dodoString request = statements[STATEMENT_UPDATE];

    request.append(collectedData.condition._table);
    request.append(statements[STATEMENT_SET]);

    dodoArray<dodoStringArray>::iterator v = collectedData.rows.values.begin();
    if (v != collectedData.rows.values.end()) {
        dodoStringArray::const_iterator i(collectedData.rows.fields.begin()), j(v->begin());
        if (i != j) {
            unsigned int fn(collectedData.rows.fields.size()), fv(v->size());

            unsigned int o(fn <= fv ? fn : fv);

            dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.condition._table);
            if (types != fieldTypes.end()) {
                dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator type;
                dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

                --o;
                for (unsigned int k(0); k < o; ++i, ++k, ++j) {
                    request.append(*i);

                    type = types->second.find(*i);
                    if (type != typesEnd) {
                        if (type->second == FIELD_TEXT || type->second == FIELD_BINARY) {
                            request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
                            request.append(escapeFields(*j));
                            request.append(statements[STATEMENT_APOSTROPHECOMA]);
                        } else {
                            request.append(statements[STATEMENT_EQUAL]);
                            request.append(*j);
                            request.append(statements[STATEMENT_COMA]);
                        }
                    } else {
                        request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
                        request.append(escapeFields(*j));
                        request.append(statements[STATEMENT_APOSTROPHECOMA]);
                    }
                }
                request.append(*i);

                type = types->second.find(*i);
                if (type != typesEnd) {
                    if (type->second == FIELD_TEXT || type->second == FIELD_BINARY) {
                        request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
                        request.append(escapeFields(*j));
                        request.append(statements[STATEMENT_APOSTROPHE]);
                    } else {
                        request.append(statements[STATEMENT_EQUAL]);
                        request.append(*j);
                    }
                } else {
                    request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
                    request.append(escapeFields(*j));
                    request.append(statements[STATEMENT_APOSTROPHE]);
                }
            } else {
                --o;
                for (unsigned int k(0); k < o; ++i, ++k, ++j) {
                    request.append(*i);
                    request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
                    request.append(escapeFields(*j));
                    request.append(statements[STATEMENT_APOSTROPHECOMA]);
                }
                request.append(*i);
                request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
                request.append(escapeFields(*j));
                request.append(statements[STATEMENT_APOSTROPHE]);
            }
        }
    }

    if (collectedData.condition._statement.size() > 0) {
        request.append(statements[STATEMENT_WHERE]);
        request.append(collectedData.condition._statement);
    }

    if (collectedData.condition._orderby.size() > 0) {
        request.append(statements[STATEMENT_ORDERBY]);
        request.append(collectedData.condition._orderby);
    }

    if (collectedData.condition._limit != -1) {
        request.append(statements[STATEMENT_LIMIT]);
        request.append(tools::string::lToString(collectedData.condition._limit));
    }

    return request;
}

//-------------------------------------------------------------------

dodoString
constructor::remove()
{
    dodoString request = statements[STATEMENT_DELETE];

    request.append(statements[STATEMENT_FROM]);
    request.append(collectedData.condition._table);

    if (collectedData.condition._statement.size() > 0) {
        request.append(statements[STATEMENT_WHERE]);
        request.append(collectedData.condition._statement);
    }

    if (collectedData.condition._orderby.size() > 0) {
        request.append(statements[STATEMENT_ORDERBY]);
        request.append(collectedData.condition._orderby);
    }

    if (collectedData.condition._limit != -1) {
        request.append(statements[STATEMENT_LIMIT]);
        request.append(tools::string::lToString(collectedData.condition._limit));
    }

    return request;
}

//-------------------------------------------------------------------

dodoString
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

dodoString
constructor::escapeFields(const dodoString &data)
{
    dodoString temp;

    unsigned long size = data.size();

    for (unsigned long i = 0; i < size; ++i) {
        if (data[i] == '\\')
            temp.append("\\\\");
        else {
            if (data[i] == '\'')
                temp.append("\\'");
            else
                temp.append(1, data[i]);
        }
    }

    return temp;
}

//-------------------------------------------------------------------

dodoString
constructor::joinFields(const dodoStringArray &fields,
                        const dodoString      &separator,
                        const dodoString      &frame,
                        int                   limit)
{
    int k(0);

    dodoString temp, fs(frame + separator);
    dodoStringArray::const_iterator i(fields.begin()), j(fields.end());
    if (i != j) {
        --j;
        for (; i != j; ++i) {
            if (limit != -1) {
                if (k > limit)
                    return temp;
                ++k;
            }
            temp.append(frame);
            temp.append(escapeFields(*i));
            temp.append(fs);
        }
        temp.append(frame);
        temp.append(escapeFields(*i));
        temp.append(frame);
    }

    return temp;
}

//-------------------------------------------------------------------
