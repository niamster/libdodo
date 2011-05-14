/***************************************************************************
 *            the list.h
 *
 *  Sat May  14 2011
 *  Copyright  2011  Dmytro Milinevskyy
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

#ifndef _LIST_H_
#define _LIST_H_ 1

#include <libdodo/directives.h>

namespace dodo {
    /**
     * @class slList
     * @brief defines basic single-linked the list type
     */
    template <typename T>
    class slList {
      public:

        /**
         * @struct element
         * @brief defines single-linked the list element
         */
        struct element {
            element *next; ///< next element in the list
            T *data; ///< pointer to the data
            bool own; //< true if allocated internally data for element
        };

        /**
         * constructor
         */
        slList() : elements(0)
        {
            head.next = &head;
        }

        /**
         * copy constructor
         */
        slList(const slList &l)
        {
            element *o;

            for (o=l.head.next; o!=&l.head; o=o->next) {
                push(*o->data);
            }
        }

        /**
         * @return length of the list
         */
        inline unsigned long size() const { return elements; }

        /**
         * pushes an element to the head of the list
         * @param t defines the element to add to the list
         */
        inline void push(const T &t)
        {
            element *e = new element;

            e->own = true;
            e->data = new T(t);

            e->next = &head;
            head.next = e;

            ++elements;
        }

        /**
         * pushes an element to the head of the list
         * @param t defines the element to add to the list
         * @note element is not copied
         */
        inline void push(const T *t)
        {
            element *e = new element;

            e->own = false;
            e->data = t;

            e->next = &head;
            head.next = e;

            ++elements;
        }

        /**
         * removes an element from the head of the list
         * TODO: do not delete data by request if the list is owner
         */
        inline void pop()
        {
            element *e = head.next;

            head.next = e->next;

            if (e->own)
                delete e->data;
            delete e;

            --elements;
        }

        /**
         * clears the list
         */
        void clear()
        {
            while (elements)
                pop();
        }

        class iterator {
            friend class slList;

          public:

            /**
             * constructor
             */
            iterator() : curr(0), prev(0) {}

            /**
             * constructor
             */
            iterator(const element *e) : curr((element *)e), prev((element *)e) {}

            /**
             * constructor
             */
            iterator(const iterator &i) : curr((element *)i.curr), prev((element *)i.prev) {}

            /**
             * dereference operator
             * @return element's data
             */
            inline T operator *() { return *curr->data; }

            /**
             * dereference operator
             * @return pointer to element's data
             */
            inline T * operator ->() { return curr->data; }

            /**
             * postfix ++ operator
             * @return old element
             */
            inline const iterator & operator++(int)
            {
                iterator i = *this;
                prev = curr;
                curr = curr->next;
                return i;
            }

            /**
             * prefix ++ operator
             * @return new element
             */
            inline const iterator & operator++()
            {
                prev = curr;
                curr = curr->next;
                return *this;
            }

            /**
             * equal operator
             * @param i defines element to compare
             */
            inline bool operator==(const iterator &i)
            {
                return curr == i.curr;
            }

            /**
             * equal operator
             * @param i defines element to compare
             */
            inline bool operator!=(const iterator &i)
            {
                return curr != i.curr;
            }

          private:

            element *curr; ///< current element of the list
            element *prev; ///< previous element of the list
        };

        /**
         * removes element from the list
         * @param i defines element to remove
         * TODO: update iterator
         */
        void remove(const iterator &i)
        {
            element *e = i.curr;

            i.prev->next = e->next;

            if (e->own)
                delete e->data;
            delete e;

            --elements;
        }

        /**
         * removes element from the list by value
         * @param t defines element to remove
         * TODO: do not delete data by request if the list is owner
         */
        void remove(const T &t)
        {
            element *e, *p;

            for (p=&head, e=head.next; e!=&head; p=e, e=e->next) {
                if (t == *e->data) {
                    p->next = e->next;

                    if (e->own)
                        delete e->data;
                    delete e;

                    --elements;

                    break;
                }
            }
        }

#if 0
        /**
         * removes element from the list by pointer
         * @param t defines element to remove
         */
        void remove(const T *t)
        {
            element *o, *p;

            for (p=&head, e=head.next; e!=&head; p=e, e=e->next) {
                if (t == e->data) {
                    p->next = e->next;

                    if (e->own)
                        delete e->data;
                    delete e;

                    --elements;

                    break;
                }
            }
        }
#endif

        /**
         * @return iterator to the first element
         */
        inline iterator begin() { return iterator(head.next); }

        /**
         * @return iterator to the first element
         */
        inline iterator begin() const { return iterator(head.next); }

        /**
         * @return iterator to the list end
         */
        inline iterator end() { return iterator(&head); }

        /**
         * @return iterator to the list end
         */
        inline iterator end() const { return iterator(&head); }

      private:

        element head; ///< head of the list
        unsigned long elements; ///< amount of elements in the list
    };

    /**
     * @class dlList
     * @brief defines basic double-linked the list type
     */
    template <typename T>
    class dlList {
      public:

        /**
         * @struct element
         * @brief defines double-linked the list element
         */
        struct element {
            element *next; ///< next element in the list
            element *prev; ///< previous element in the list
            T *data; ///< pointer to the data
            bool own; //< true if allocated internally data for element
        };

        /**
         * constructor
         */
        dlList() : elements(0)
        {
            head.next = head.prev = &head;
        }

        /**
         * copy constructor
         */
        dlList(const dlList &l)
        {
            element *o;

            for (o=l.head.next; o!=&l.head; o=o->next) {
                pushBack(*o->data);
            }
        }

        /**
         * @return length of the list
         */
        inline unsigned long size() const { return elements; }

        /**
         * pushes an element to the tail of the list
         * @param t defines the element to add to the list
         */
        inline void pushBack(const T &t)
        {
            element *e = new element;

            e->own = true;
            e->data = new T(t);

            head.prev->next = e;
            e->prev = head.prev;
            head.prev = e;
            e->next = &head;

            ++elements;
        }

        /**
         * pushes an element to the tail of the list
         * @param t defines the element to add to the list
         * @note element is not copied
         */
        inline void pushBack(const T *t)
        {
            element *e = new element;

            e->own = false;
            e->data = t;

            head.prev->next = e;
            e->prev = head.prev;
            head.prev = e;
            e->next = &head;

            ++elements;
        }

        /**
         * pushes an element to the head of the list
         * @param t defines the element to add to the list
         */
        inline void pushFront(const T &t)
        {
            element *e = new element;

            e->own = true;
            e->data = new T(t);

            head.next->prev = e;
            e->next = head.next;
            head.next = e;
            e->prev = &head;

            ++elements;
        }

        /**
         * pushes an element to the head of the list
         * @param t defines the element to add to the list
         * @note element is not copied
         */
        inline void pushFront(const T *t)
        {
            element *e = new element;

            e->own = false;
            e->data = t;

            head.next->prev = e;
            e->next = head.next;
            head.next = e;
            e->prev = &head;

            ++elements;
        }


        /**
         * Removes an element from the head of the list
         */
        inline void popFront()
        {
            element *e = head.next;

            head.next = e->next;
            head.next->prev = &head;

            if (e->own)
                delete e->data;
            delete e;

            --elements;
        }

        /**
         * Removes an element from the tail of the list
         */
        inline void popBack()
        {
            element *e = head.prev;

            head.prev = e->prev;
            head.prev->next = &head;

            if (e->own)
                delete e->data;
            delete e;

            --elements;
        }

        /**
         * clears the list
         */
        void clear()
        {
            while (elements)
                popBack();
        }

        class iterator {
          public:

            /**
             * constructor
             */
            iterator() : curr(0) {}

            /**
             * constructor
             */
            iterator(const element *e) : curr((element *)e) {}

            /**
             * constructor
             */
            iterator(const iterator &i) : curr((element *)i.curr) {}

            /**
             * dereference operator
             * @return element's data
             */
            inline T operator *() { return *curr->data; }

            /**
             * dereference operator
             * @return pointer to element's data
             */
            inline T * operator ->() { return curr->data; }

            /**
             * postfix ++ operator
             * @return old element
             */
            inline const iterator & operator++(int)
            {
                iterator i = *this;
                curr = curr->next;
                return i;
            }

            /**
             * prefix ++ operator
             * @return new element
             */
            inline const iterator & operator++()
            {
                curr = curr->next;
                return *this;
            }

            /**
             * equal operator
             * @param i defines element to compare
             */
            inline bool operator==(const iterator &i)
            {
                return curr == i.curr;
            }

            /**
             * equal operator
             * @param i defines element to compare
             */
            inline bool operator!=(const iterator &i)
            {
                return curr != i.curr;
            }

          private:

            element *curr; ///< curr element of the list
        };

        /**
         * removes element from the list
         * @param i defines element to remove
         */
        void remove(const iterator &i)
        {
            element *e = i.curr;

            e->prev->next = e->next;
            e->next->prev = e->prev;

            if (e->own)
                delete e->data;
            delete e;

            --elements;
        }

        /**
         * removes element from the list by value
         * @param t defines element to remove
         */
        void remove(const T &t)
        {
            element *e;

            for (e=head.next; e!=&head; e=e->next) {
                if (t == *e->data) {
                    e->prev->next = e->next;
                    e->next->prev = e->prev;

                    if (e->own)
                        delete e->data;
                    delete e;

                    --elements;

                    break;
                }
            }
        }

#if 0
        /**
         * removes element from the list by pointer
         * @param t defines element to remove
         */
        void remove(const T *t)
        {
            element *e;

            for (e=head.next; e!=&head; e=e->next) {
                if (t == e->data) {
                    e->prev->next = e->next;
                    e->next->prev = e->prev;

                    if (e->own)
                        delete e->data;
                    delete e;

                    --elements;

                    break;
                }
            }
        }
#endif

        /**
         * @return iterator to the first element
         */
        inline iterator begin() { return iterator(head.next); }

        /**
         * @return iterator to the first element
         */
        inline iterator begin() const { return iterator(head.next); }

        /**
         * @return iterator to the list end
         */
        inline iterator end() { return iterator(&head); }

        /**
         * @return iterator to the list end
         */
        inline iterator end() const { return iterator(&head); }

      private:

        element head; ///< head of the list
        unsigned long elements; ///< amount of elements in the list
    };
};
#endif
