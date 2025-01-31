import wtforms
from wtforms.validators import InputRequired, NumberRange
from flask_wtf import FlaskForm
from datetime import date
class CSSCForm(FlaskForm): # charging station speed category
    
    #the charging power per chargepoint (default: 11 kW)
    charging_power = wtforms.IntegerField(label="Charging Power in kW",
        validators=[InputRequired(), NumberRange(min=1)],
        default=11
    )
                         
    #the number of charge points
    charging_station_count = wtforms.IntegerField(label="Number of charging stations",
        validators=[InputRequired(), NumberRange(min=1)],
        default=20
    )
class FormBasics(CSSCForm):

    tag = wtforms.StringField(label="Tag", default=f"Set of inputs {date.today()}")
    #a multiplier for the arrival probability (20-200%, default: 100%)
    arrival_pr_mult = wtforms.FloatField(label="Multiplier for arrivals (in percent)",
        validators=[InputRequired(), NumberRange(min=20, max=200)],
        default=100
    )
    charging_speed = wtforms.IntegerField(label="EV charging speed (kW)",
        validators=[InputRequired(), NumberRange(min=1)],
        default=11
    )
    charging_station_count = wtforms.IntegerField(label="Amount of charging stations",
        validators=[InputRequired(), NumberRange(min=1, max=10000)],
        default=20
    )
    ticks_per_hour = wtforms.IntegerField(label="Simulation ticks per hour",
        validators=[InputRequired(), NumberRange(min=1)],
        default=4
    )

    simulation_duration = wtforms.IntegerField(label="Amount of days simulated",
        validators=[InputRequired(), NumberRange(min=1, max=3650)],
        default=365
    )

    random_seed = wtforms.IntegerField(label="Seed for the random values",
        validators=[NumberRange(min=0)],
        default=2025
    )
    #the consumption of the cars (default: 18 kWh)
    car_efficiency = wtforms.IntegerField(label="Car efficiency (kWh per 100km)",
        validators=[InputRequired(), NumberRange(min=1)],
        default=18
    )
class BasicForm(FormBasics, CSSCForm):
    pass
class MultiForm(FormBasics):
    """Parent form."""
    #TODO add validators
    #                             
    csscs = wtforms.FieldList(
        wtforms.FormField(CSSCForm),
        min_entries=1,
        max_entries=20
    )