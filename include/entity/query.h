#pragma once


namespace ent
{
	class qbase
	{
		public:
			virtual ~qbase() {}
			
		protected:
			std::shared_ptr<qbase> parent = nullptr;
			template <class T> friend class query;
	};


	template <class T> class query : public qbase
	{
		public:

			// Force the default copy constructor (to avoid getting caught by the templated one below)
			query(const query<T> &copy) = default;

			// Construct from initialiser list
			query(std::initializer_list<T> data) : buffer(std::make_shared<std::vector<T>>(data))
			{
				this->setup(*this->buffer);
			}

			// Construct from any container type
			template <class U, class = typename std::enable_if<std::is_same<T, typename U::value_type>::value>::type> query(U &data)
			{
				this->setup(data);
			}


			query<T> where(std::function<bool(const T&)> predicate)
			{
				return {
					*this,
					[=](qbase *c, bool forward) -> T* {
						auto p = static_cast<query<T>*>(c->parent.get());

						for (T *i = p->start(p, forward); i; i = p->next(p)) if (predicate(*i)) return i;

						return nullptr;
					},
					[=](qbase *c) -> T* {
						auto p = static_cast<query<T>*>(c->parent.get());

						for (T *i = p->next(p); i; i = p->next(p)) if (predicate(*i)) return i;

						return nullptr;
					}
				};
			}


			query<T> take(int count)
			{
				return {
					*this,
					[=](qbase *c, bool forward) {
						auto p = static_cast<query<T>*>(c->parent.get());

						static_cast<query<T>*>(c)->counter = 1;

						return count > 0 ? p->start(p, forward) : nullptr;
					},
					[=](qbase *c) {
						auto t = static_cast<query<T>*>(c);
						auto p = static_cast<query<T>*>(c->parent.get());

						return t->counter++ < count ? p->next(p) : nullptr;
					}
				};
			}


			query<T> take_while(std::function<bool(const T&)> predicate)
			{
				return {
					*this,
					[=](qbase *c, bool forward) {
						auto p 	= static_cast<query<T>*>(c->parent.get());
						T *i	= p->start(p, forward);

						return predicate(*i) ? i : nullptr;
					},
					[=](qbase *c) {
						auto p 	= static_cast<query<T>*>(c->parent.get());
						T *i	= p->next(p);

						return predicate(*i) ? i : nullptr;
					}
				};
			}


			query<T> reverse()
			{
				return {
					*this,
					[](qbase *c, bool forward) {
						return static_cast<query<T>*>(c->parent.get())->start(c->parent.get(), !forward);
					},
					[](qbase *c) {
						return static_cast<query<T>*>(c->parent.get())->next(c->parent.get());
					}
				};
			}


			template <class U> query<U> select(std::function<U(const T&)> operation)
			{
				return query<U> {
					*this,
					[=](qbase *c, bool forward) {
						auto t	= static_cast<query<U>*>(c);
						auto p	= static_cast<query<T>*>(c->parent.get());
						T *i	= p->start(p, forward);

						return i ? &(t->item = operation(*i)) : nullptr;
					},
					[=](qbase *c) {
						auto t	= static_cast<query<U>*>(c);
						auto p	= static_cast<query<T>*>(c->parent.get());
						T *i	= p->next(p);

						return i ? &(t->item = operation(*i)) : nullptr;
					}
				};
			}


			template <class U> query<T> order_by(std::function<U(const T&)> selector, bool descending = false)
			{
				return {
					*this,
					[=](qbase *c, bool forward) {
						auto t	= static_cast<query<U>*>(c);
						auto p	= static_cast<query<T>*>(c->parent.get());

						t->buffer->clear();
						for (T *i = p->start(p, true); i; i = p->next(p)) t->buffer->push_back(*i);

						std::sort(t->buffer->begin(), t->buffer->end(), [&](const T &a, const T &b) { return selector(a) < selector(b); });

						if (forward == !descending) static_cast<query<T>*>(c)->it = std::make_shared<iterators<typename std::vector<T>::iterator>>(t->buffer->begin(), t->buffer->end());
						else						static_cast<query<T>*>(c)->it = std::make_shared<iterators<typename std::vector<T>::reverse_iterator>>(t->buffer->rbegin(), t->buffer->rend());

						return t->it->next();
					},
					[=](qbase *c) {
						return static_cast<query<T>*>(c)->it->next();
					},
					std::make_shared<std::vector<T>>()
				};
			}


			T aggregate(std::function<T(const T&, const T&)> accumulator)
			{
				return std::accumulate(++this->begin(), this->end(), *this->begin(), accumulator);
			}


			template <class U> U aggregate(U seed, std::function<U(const U&, const T&)> accumulator)
			{
				return std::accumulate(this->begin(), this->end(), seed, accumulator);
			}


			template <class U, class V> V aggregate(U seed, std::function<U(const U&, const T&)> accumulator, std::function<V(const U&)> selector)
			{
				return selector(std::accumulate(this->begin(), this->end(), seed, accumulator));
			}


			bool all(std::function<bool(const T&)> predicate)
			{
				for (T *i = start(this, true); i; i = next(this)) if (!predicate(*i)) return false;
				
				return true;
			}


			bool any(std::function<bool(const T&)> predicate)
			{
				for (T *i = start(this, true); i; i = next(this)) if (predicate(*i)) return true;

				return false;
			}


			bool contains(const T &item)
			{
				for (T *i = start(this, true); i; i = next(this)) if (*i == item) return true;

				return false;
			}


			int count(std::function<bool(const T&)> predicate = nullptr)
			{
				int result = 0;

				if (predicate)
				{
					for (T *i = start(this, true); i; i = next(this)) if (predicate(*i)) result++;
				}
				else for (T *i = start(this, true); i; i = next(this), result++);

				return result;
			}


			T first()														{ return this->first_last(nullptr, true, true); }
			T first_or_default()											{ return this->first_last(nullptr, false, true); }
			T first(std::function<bool(const T&)> predicate)				{ return this->first_last(predicate, true, true); }
			T first_or_default(std::function<bool(const T&)> predicate)		{ return this->first_last(predicate, false, true); }

			T last()														{ return this->first_last(nullptr, true, false); }
			T last_or_default()												{ return this->first_last(nullptr, false, false); }
			T last(std::function<bool(const T&)> predicate)					{ return this->first_last(predicate, true, false); }
			T last_or_default(std::function<bool(const T&)> predicate)		{ return this->first_last(predicate, false, false); }

			T single() 														{ return this->single(nullptr, true); }
			T single_or_default()											{ return this->single(nullptr, false); }
			T single(std::function<bool(const T&)> predicate)				{ return this->single(predicate, true); }
			T single_or_default(std::function<bool(const T&)> predicate)	{ return this->single(predicate, false); }

			T min()															{ return this->min_max<T>(nullptr, true); }
			T max()															{ return this->min_max<T>(nullptr, false); }
			double average()												{ return average<T>(nullptr); }
			template <class U> U min(std::function<U(const T&)> selector)	{ return this->min_max<U>(selector, true); }
			template <class U> U max(std::function<U(const T&)> selector)	{ return this->min_max<U>(selector, false); }

			query<T> skip(int count)										{ return this->skip(count, nullptr); }
			query<T> skip_while(std::function<bool(const T&)> predicate)	{ return this->skip(0, predicate); }


			template <class U> double average(std::function<U(const T&)> selector)
			{
				static_assert(std::is_arithmetic<U>::value, "query::average is only suitable for arithmetic types");

				int count 	= 0;
				double sum	= 0;

				if (selector) 	for (T *i = start(this, true); i; i = next(this), count++) sum += (double)selector(*i);
				else			for (T *i = start(this, true); i; i = next(this), count++) sum += (double)*i;

				if (!count) throw std::runtime_error("query::average invalid since query result is empty");

				return sum / (double)count;
			}


			std::vector<T> vector()
			{
				std::vector<T> result;

				for (T *i = start(this, true); i; i = next(this)) result.emplace_back(*i);

				return result;
			}


			template <class U, class V> std::map<U, V> map(std::function<U(T&)> key, std::function<V(T&)> value)
			{
				std::map<U, V> result;

				for (T *i = start(this, true); i; i = next(this)) result[key(*i)] = value(*i);

				return result;
			}


			
			struct iterator : std::iterator<std::input_iterator_tag, T>
			{
				iterator() {}

				iterator(const query<T> &q, bool forward) : q(q)
				{
					this->current = this->q.start(&this->q, forward);
				}

				iterator &operator++()
				{
					this->current = this->q.next(&this->q);
					return *this;
				}

				bool operator!=(iterator &i) 	{ return i.current != this->current; }
				bool operator==(iterator &i)	{ return i.current == this->current; }
				T &operator*()					{ return *this->current; }

				private:
					query<T> q;
					T *current = nullptr;
			};

			iterator begin()	{ return iterator(*this, true); }
			iterator end()		{ return iterator(); }
			iterator rbegin()	{ return iterator(*this, false); }
			iterator rend()		{ return iterator(); }

		private:
	
			struct ibase
			{
				virtual T *next() = 0;
			};

			template<class C> struct iterators : ibase
			{
				C current, end;

				iterators(C start, C end) : current(start), end(end) {}

				T *next() { return (current != end) ? &*current++ : nullptr; }
			};


			query() {}

			template <class U> query(query<U> &parent, std::function<T *(qbase *, bool)> start, std::function<T *(qbase *)> next, std::shared_ptr<std::vector<T>> buffer = nullptr)
			{
				this->parent	= std::make_shared<query<U>>(parent);
				this->start		= start;
				this->next		= next;
				this->buffer	= buffer;
			}


			// Used by the query instance that first referenced the source container data.
			// The initialise function will store the begin and end iterators for the given container
			// The next function is used to iterator over the items in the container
			template <class U> void setup(U &data)
			{
				this->start = [&](qbase *c, bool forward) {
					if (forward) 	static_cast<query<T>*>(c)->it = std::make_shared<iterators<typename U::iterator>>(data.begin(), data.end());
					else			static_cast<query<T>*>(c)->it = std::make_shared<iterators<typename U::reverse_iterator>>(data.rbegin(), data.rend());
					return 			static_cast<query<T>*>(c)->it->next();
				};

				this->next = [](qbase *c) {
					return static_cast<query<T>*>(c)->it->next();
				};
			}


			T first_last(std::function<bool(const T&)> predicate, bool except, bool forward)
			{
				T *i = start(this, forward);

				if (predicate) for (; i && !predicate(*i); i = next(this));

				if (except && !i) throw std::runtime_error("query::first/last invalid since query result is empty");

				return i ? *i : T();
			}


			T single(std::function<bool(const T&)> predicate, bool except)
			{
				T *i = start(this, true);

				if (predicate)		for(; i && !predicate(*i); i = next(this));
				if (except && !i)	throw std::runtime_error("query::single invalid since query result is empty");

				T *n = next(this);

				if (predicate)	for (; n && !predicate(*n); n = next(this));
				if (n)			throw std::runtime_error("query::single more than one result");

				return i ? *i : T();
			}


			template <class U> U min_max(std::function<U(const T&)> selector, bool min)
			{
				T *i = start(this, true);
				if (!i) throw std::runtime_error("query::min/max invalid since query result is empty");

				U result = selector ? selector(*i) : *i;

				if (min)	for (T *i = next(this); i; i = next(this)) result = std::min(result, selector ? selector(*i) : *i);
				else		for (T *i = next(this); i; i = next(this)) result = std::max(result, selector ? selector(*i) : *i);

				return result;
			}


			query<T> skip(int count, std::function<bool(const T&)> predicate)
			{
				return {
					*this,
					[=](qbase *c, bool forward) {
						auto p 	= static_cast<query<T>*>(c->parent.get());
						T *i	= p->start(p, forward);

						if (predicate)	for (; i && predicate(*i); i = p->next(p));
						else			for (int j=0; i && j<count; i = p->next(p), j++);

						return i;
					},
					[](qbase *c) {
						return static_cast<query<T>*>(c->parent.get())->next(c->parent.get());
					}
				};
			}


			std::function<T *(qbase *, bool)> start;	// Initialises this link in the query chain and returns the first item
			std::function<T *(qbase *)> next;			// Gets the next item from the chain

			std::shared_ptr<ibase> it;				// Iterator storage
			std::shared_ptr<std::vector<T>> buffer;	// Buffer used for storing initializer_list data
			T item;									// Temporary used to return a reference to a modified value
			int counter;

			template <class U> friend class query;
	};


	template <typename T> struct is_container
	{
		private:
			template <typename U> static char test(typename U::iterator*);
			template <typename U> static int test(...);

		public:
			enum { value = sizeof(test<T>(0)) == sizeof(char) };
	};


	template <class T, class = typename std::enable_if<is_container<T>::value>::type> query<typename T::value_type> from(T &data) { return query<typename T::value_type>(data); }
	template <class T> query<T> from(std::initializer_list<T> data) { return query<T>(data); }
}


/*
-aggregate(reducer)
-aggregate(seed, reducer)
-aggregate(seed, reducer, selector)
-all(predicate)
-any(predicate)
-average()
concat(range)
-contains(element)
-count()
-count(predicat)
default_if_empty()
default_if_empty(default_value)
distinct()
element_at(index)
except(range)
find(element)
-first()
-first(predicate, value)
-first_or_default()
-first_or_default(predicate)
group_by(key_selector)
group_by(key_selector, element_selector)
group_join(range, outer_key_selector, inner_key_selector, result_selector)
intersect(range)
join(range, outer_key_selector, inner_key_selector, result_selector)
keys()
-last()
-last(predicate, value)
-last_or_default()
-last_or_default(predicate)
-max()
-min()
-order_by(selector)
-order_by_descending(selector)
-reverse()
-select(selector)
select_many(selector)
sequence_equal(range)
-single()
-single_or_default()
-skip(count)
-skip_while(predicate)
sum()
-take(count)
-take_while(predicate)
then_by(selector)
then_by_descending(selector)
-to_container()
union(range)
values()
-where(predicate)
zip(range)
zip(range, selector)
*/
