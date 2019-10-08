from flask_wtf import FlaskForm
from wtforms import StringField, PasswordField, BooleanField, SubmitField, \
    SelectField, HiddenField, SelectMultipleField, TextAreaField, DecimalField, FileField
from wtforms.validators import DataRequired, EqualTo, ValidationError, Length

from shop.models import User, load_user, load_address, load_categoris


class LoginForm(FlaskForm):
    username = StringField('Username', validators=[DataRequired()])
    password = PasswordField('Password', validators=[DataRequired()])
    submit = SubmitField('Sign In')


class RegistrationForm(FlaskForm):
    username = StringField('Username', validators=[DataRequired(), Length(min=5, max=20)])
    password = PasswordField('Password', validators=[DataRequired(), Length(min=5, max=20)])
    password2 = PasswordField(
        'Repeat Password', validators=[DataRequired(), EqualTo('password')])
    submit = SubmitField('Register')

    def validate_username(self, username):
        user = load_user(username=username.data)
        if user is not None:
            raise ValidationError('Please use a different username.')


class OrderCreateForm(FlaskForm):
    name = StringField('name', validators=[DataRequired()])
    phone = StringField('phone', validators=[DataRequired(), Length(min=11, max=11)])
    address = StringField('address', validators=[DataRequired()])

    submit = SubmitField('Place order')


PRODUCT_QUANTITY_CHOICES = [(i, str(i)) for i in range(1, 21)]


class CartAddProductForm(FlaskForm):
    num = SelectField('num', validators=[DataRequired()], choices=PRODUCT_QUANTITY_CHOICES, coerce=int)
    update = HiddenField('update', validators=[DataRequired()], default=False)

    submit = SubmitField('Add to cart')


class EditProfileForm(FlaskForm):
    name = StringField('name', validators=[DataRequired()])
    sex = SelectField('Sex', choices=[('M', 'Male'), ('F', 'Female')])
    phone = StringField('Phone', validators=[DataRequired(), Length(min=11, max=11)])

    submit = SubmitField('Submit')


class EditOrderTransportForm(FlaskForm):
    trans = StringField('Trans Id', validators=[DataRequired()])

    submit = SubmitField('Submit')


class ProductForm(FlaskForm):
    choices = [(choice['id'], choice['name']) for choice in load_categoris()]
    cate = SelectField('Category', choices=choices)
    name = StringField('Name', validators=[DataRequired()])
    price = DecimalField('Price', validators=[DataRequired()])
    image = FileField('Image')
    description = StringField('Description', validators=[DataRequired()])

    submit = SubmitField('Submit')
