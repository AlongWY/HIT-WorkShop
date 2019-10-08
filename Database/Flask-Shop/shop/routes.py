from shop import app
from config import query

from flask import request
from werkzeug.urls import url_parse
from flask import render_template, flash, redirect, url_for
from flask_login import login_user, logout_user, current_user, login_required

from shop.models import load_user, User, register_user, load_product, load_category, Cart, Order, update_user, \
    load_order, pay_it_for_order, load_order_items, set_order_trans
from shop.forms import LoginForm, RegistrationForm, CartAddProductForm, OrderCreateForm, EditProfileForm, \
    EditOrderTransportForm, ProductForm


@app.route('/')
@app.route('/<cate_id>')
def index(cate_id=None):
    if cate_id is not None:
        try:
            cate_id = int(cate_id)
        except:
            return "No this Cate"

    result = query("select * from category").fetchall()
    categories = [dict(row.items()) for row in result]

    if (cate_id is None):
        sql = "select * from product"
    else:
        sql = f"select * from product where cate_id = {cate_id}"
    result = query(sql).fetchall()
    products = [dict(row.items()) for row in result]

    if cate_id is not None:
        category = [cate for cate in categories if cate['id'] == cate_id]
        for cate in category:
            return render_template('shop/product/list.html', category=cate,
                                   categories=categories, products=products)

    return render_template('shop/product/list.html', category={'slug': cate_id},
                           categories=categories, products=products)


@app.route('/<cate_id>/<product_id>')
def product_details(cate_id=None, product_id=None):
    category = load_category(cate_id)
    if category is None:
        return "No This Product!!!!"
    product = load_product(product_id)
    if product is None:
        return "No This Product!!!!"
    cart_product_form = CartAddProductForm()
    return render_template('shop/product/detail.html',
                           category=category, product=product, form=cart_product_form)


@app.route('/login', methods=['GET', 'POST'])
def login():
    form = LoginForm()
    if form.validate_on_submit():
        user = load_user(form.username.data)
        if user is None or not user.check_password(form.password.data):
            flash('Invalid username or password')
            return redirect(url_for('login'))
        login_user(user, remember=True)
        Cart(True)
        next_page = request.args.get('next')
        if not next_page or url_parse(next_page).netloc != '':
            next_page = url_for('index')
        return redirect(next_page)
    return render_template('shop/login.html', title='Sign In', form=form)


@app.route('/logout')
def logout():
    logout_user()
    return redirect(url_for('index'))


@app.route('/register', methods=['GET', 'POST'])
def register():
    if current_user.is_authenticated:
        return redirect(url_for('index'))
    form = RegistrationForm()
    if form.validate_on_submit():
        info = {
            'username': form.username.data,
            'password': form.password.data
        }
        user = User(info)
        register_user(user)
        flash('Congratulations, you are now a registered user!')
        return redirect(url_for('login'))
    return render_template('shop/register.html', title='Register', form=form)


@app.route('/<cate_id>/<product_id>/cart_add', methods=['POST'])
def cart_add(cate_id, product_id):
    form = CartAddProductForm()
    cart = Cart()
    product = load_product(product_id)
    if form.validate_on_submit():
        cart.add(product=product, num=form.num.data, update_num=form.update.data)
    return redirect(url_for('cart_detail'))


@app.route('/cart', methods=['GET'])
def cart_detail():
    return render_template('shop/cart/detail.html')


@app.route('/<cate_id>/<product_id>/cart_remove', methods=['GET'])
def cart_remove(cate_id, product_id):
    cart = Cart()
    product = load_product(product_id)
    if product:
        cart.remove(product)
    return redirect(url_for('cart_detail'))


@app.route('/order_create', methods=['GET', 'POST'])
@login_required
def order_create():
    form = OrderCreateForm()
    if form.validate_on_submit():
        cart = Cart()
        if len(cart) == 0:
            return redirect(url_for('index'))
        order = Order(form.name.data, form.phone.data, form.address.data)
        order.create(cart)
        cart.clear()
        return render_template('shop/cart/created.html', form=form, order=order)
    return render_template('shop/cart/create.html', form=form)


@app.route('/profile', methods=['GET'])
@login_required
def profile():
    user_ = load_user()
    orders = load_order()
    return render_template('shop/profile/detail.html', user=user_, orders=orders)


@app.route('/profile/edit', methods=['GET', 'POST'])
@login_required
def edit_profile():
    form = EditProfileForm()
    if form.validate_on_submit():
        update_user(form.name.data, form.sex.data, form.phone.data)
        return redirect(url_for('edit_profile'))
    return render_template('shop/profile/edit.html', title='Edit Profile', form=form)


@app.route('/pay', methods=['GET', 'POST'])
@login_required
def pay_for_order():
    data = request.args.to_dict()
    pay_it_for_order(data['order_id'])
    return redirect(url_for('profile'))


@app.route('/order/detail', methods=['GET', 'POST'])
def order_detail():
    data = request.args.to_dict()
    if 'id' not in data:
        return redirect(url_for('index'))
    order, items = load_order_items(order_id=data['id'])

    form = EditOrderTransportForm()
    if form.validate_on_submit():
        set_order_trans(order['id'], form.trans.data)
    return render_template('shop/order/detail.html', order=order, items=items, form=form)


@app.route('/product/edit', methods=['GET', 'POST'])
def product_edit():
    data = request.args.to_dict()
    if 'id' not in data:
        return redirect(url_for('index'))
    form = ProductForm()
    if form.validate_on_submit():
        pass
    return render_template('shop/product/edit.html', product_form=form)
