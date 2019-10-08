from decimal import Decimal
from flask_login import UserMixin, current_user
from flask import session
from sqlalchemy import text

from config import query, execute, Config
from shop import login


class User(UserMixin):
    def __init__(self, user_info):
        self.info = user_info
        self.username = user_info["username"]
        self.password = user_info["password"]

    def get_id(self):
        return self.username

    def check_password(self, pwd):
        return self.password == pwd

    def set_id(self, user_id):
        self.username = user_id

    def set_password(self, password_hash):
        self.password = password_hash

    @property
    def is_admin(self):
        return self.username == 'admin'


class Cart(object):
    def __init__(self, update=False):
        self.session = session
        cart: dict = self.session.get(Config.CART_SESSION_ID)

        if update:
            username = self.session.get('user_id')
            if username:
                self.cart = load_cart(username)
                if self.cart:
                    for product_id, product_value in cart.items():
                        product = load_product(product_id)
                        self.add(product, product_value['num'], update_num=True)
                else:
                    self.cart = {}
        if not cart:
            username = self.session.get('user_id')
            if username:
                cart = load_cart(username)
            else:
                cart = self.session[Config.CART_SESSION_ID] = {}
        if not update:
            self.cart = cart

    def __len__(self):
        return sum(item['num'] for item in self.cart.values())

    def add(self, product, num=1, update_num=False):
        product['id'] = str(product['id'])
        if product['id'] not in self.cart:
            self.cart[product['id']] = {'num': 0, 'price': str(product['price'])}
        if update_num:
            self.cart[product['id']]['num'] = num
        else:
            self.cart[product['id']]['num'] += num
        username = self.session.get('user_id')
        if username:
            sql = text(
                'insert into cart (username, product, num) values (:username, :product, :num) on duplicate key update num = :num')
            execute(sql, {'username': username, 'product': int(product['id']), 'num': self.cart[product['id']]['num']})
        self.save()

    def save(self):
        self.session.modified = True

    def remove(self, product):
        product_id = str(product['id'])
        if product_id in self.cart:
            del self.cart[product_id]
            username = self.session.get('user_id')
            if username:
                sql = text('delete from cart where username = :username and product = :product')
                execute(sql, {'username': username, 'product': int(product['id'])})
            self.save()

    def __iter__(self):
        products = [load_product(product_id) for product_id in self.cart.keys()]

        cart = self.cart.copy()
        for product in products:
            product['id'] = str(product['id'])
            product['price'] = str(product['price'])
            cart[product['id']]['product'] = product

        for item in cart.values():
            item['total_price'] = str(Decimal(Decimal(item['price']) * item['num']))
            yield item

    @property
    def get_total_price(self):
        return sum(Decimal(Decimal(item['price']) * item['num']) for item in self.cart.values())

    def clear(self):
        if self.session.get(Config.CART_SESSION_ID):
            del self.session[Config.CART_SESSION_ID]
        username = self.session.get('user_id')
        if username:
            sql = text('delete from cart where username = :username')
            execute(sql, {'username': username})
        self.save()


class Order(object):
    def __init__(self, name, phone, address):
        self.id = None
        self.name = name
        self.phone = phone
        self.address = address

    def create(self, cart: Cart):
        sql = text("insert into order_ (price, name,"
                   "phone, address) values (:price,:name,:phone,:address)")
        result = execute(sql, {'price': cart.get_total_price, 'name': self.name, 'phone': self.phone,
                               'address': self.address})
        sql = text(
            f"insert into order_items (id, product, price, num) VALUES ({result.lastrowid}, :product, :price, :num)")
        for item in cart:
            product = item['product']
            execute(sql, {'product': product['id'], 'price': product['price'], 'num': item['num']})
        username = session.get('user_id')
        if username:
            sql = text("insert into orders (username, order_id) VALUES (:name,:id)")
            execute(sql, {'name': username, 'id': result.lastrowid})
        self.id = result.lastrowid


@login.user_loader
def load_user(username: str = None):
    if username is None:
        username = session.get('user_id')
    if not username:
        return None
    sql = text("select * from user where username = :username")
    row = query(sql, {"username": username}).fetchone()
    if row is None:
        return None
    user = dict(row.items())
    return User(user)


def load_product(product_id):
    row = query(f"select * from product where id = {product_id}").fetchone()
    if row is not None:
        return dict(row.items())
    else:
        return None


def load_category(cate_id):
    sql = text('select * from category where id = :cate')
    row = query(sql, {'cate': cate_id}).fetchone()
    if row is not None:
        return dict(row.items())
    else:
        return None


def load_categoris():
    sql = text('select * from category')
    result = query(sql).fetchall()
    if result is not None:
        return [dict(row.items()) for row in result]
    else:
        return None


def register_user(user: User):
    sql = text("INSERT INTO user (username, password) VALUES (:name , :password)")
    return execute(sql, {"name": user.username, 'password': user.password})


def load_cart(username):
    sql = text('select * from cart join product p on cart.product = p.id where username = :name')
    result = query(sql, {"name": username}).fetchall()
    if result is None:
        return []
    products = {}
    for product in [dict(row.items()) for row in result]:
        products[str(product['id'])] = {'num': product['num'], 'price': str(product['price'])}
    return products


def load_address(username):
    sql = text('select * from address where username = :name')
    result = query(sql, {'name': username})
    if result is None:
        return []
    return [dict(row.items()) for row in result]


def update_user(name, sex, phone):
    username = session.get('user_id')
    if not username:
        return None
    sql = text('update user set name = :name,sex= :sex, phone= :phone where username = :username')
    execute(sql, {'username': username, 'name': name, 'sex': sex, 'phone': phone})


def load_order():
    username = session.get('user_id')
    if not username:
        return None
    if current_user.is_admin:
        sql = text('select * from orders left join order_ on orders.order_id = order_.id')
    else:
        sql = text('select * from orders left join order_ on orders.order_id = order_.id where username = :username')
    result = query(sql, {'username': username})
    if result is None:
        return []
    return [dict(row.items()) for row in result]


def set_order_trans(order_id, trans_id):
    sql = text('update order_ set trans_id = :trans where id = :order_id')
    execute(sql, {'trans': trans_id, 'order_id': order_id})


def pay_it_for_order(order_id):
    sql = text("update order_ set state = 'd' where id = :id")
    execute(sql, {'id': order_id})


def load_order_items(order_id):
    sql = text('select * from order_  where id = :id')
    result = query(sql, {'id': order_id}).fetchone()
    if result is None:
        return []
    order = dict(result)

    sql = text('select product.cate_id, product.id, product.name,o.price,o.num from order_items o '
               'left join product on o.product = product.id where o.id = :id')
    result = query(sql, {'id': order_id}).fetchall()
    if result is None:
        return []
    return order, [dict(row.items()) for row in result]
