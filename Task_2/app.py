from flask import Flask, render_template, request
import forms
from flask_sqlalchemy import SQLAlchemy
from sqlalchemy.orm import Mapped, mapped_column
from os import path
# App config.
app = Flask(__name__)
app.config.from_object(__name__)
app.config['SECRET_KEY'] = "Shhhhh don't tell anyone ahh!"
app.config['UPLOAD_FOLDER'] = "static"
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///db.sqlite3'
app.config['SQLALCHEMY_TRACK MODIFICATIONS'] = False

db = SQLAlchemy(app)

class Input(db.Model):

    id: Mapped[int] = db.Column(db.Integer, primary_key=True)
    tag:Mapped[str] = db.Column(db.String(30))
    arrival_pr_mult: Mapped[float] = db.Column(db.Integer)
    car_efficiency: Mapped[int]= db.Column(db.Integer)
    charging_speed: Mapped[int] = db.Column(db.Integer)
    charging_station_count: Mapped[int] = db.Column(db.Integer)
    ticks_per_hour: Mapped[int]= db.Column(db.Integer)
    simulation_duration: Mapped[int]= db.Column(db.Integer)
    random_seed: Mapped[int] = db.Column(db.Integer)
    
    outputs = db.relationship('Output', backref="input")

    def __repr__(self):
        return f"<(Inp){self.tag}:arr mult:{self.arrival_pr_mult}%, car_eff:{self.car_efficiency}kWh/100km>"

class Output(db.Model):

    id:Mapped[int] = mapped_column(primary_key=True, unique=True)
    input_id = db.Column(db.Integer, db.ForeignKey('input.id'))
    total_power_consumed = db.Column(db.Float) #(in kWh)
    parking_events = db.Column(db.Integer)

    #The charging values (in kW) per chargepoint at a useful aggregation level

with app.app_context():
    db.create_all()
    print(Input.metadata.schema)
    db.session.commit()
    
@app.route("/", methods=['GET', 'POST'])
def page():
    form = forms.BasicForm()
    showres = 0
    new_outp = Output()
    new_entry = Input()
    if form.validate_on_submit():

        print("Got values: ", dict(request.form))
        new_entry = Input(  tag=request.form["tag"],
                            arrival_pr_mult= request.form["arrival_pr_mult"],
                            car_efficiency = request.form["car_efficiency"],
                            charging_speed = request.form["charging_speed"],
                            charging_station_count = request.form["charging_station_count"],
                            ticks_per_hour = request.form["ticks_per_hour"],
                            simulation_duration = request.form["simulation_duration"],
                            random_seed = request.form["random_seed"],
                            )
        db.session.add(new_entry)
        
        # 
        # CALL TO IMPLEMENTATION CODE
        #

        # store mock result 
        new_outp = Output(  input=new_entry,
                            total_power_consumed = 12345, #(in kWh)
                            parking_events = 234
                            )
        db.session.add(new_outp)
        db.session.commit()
        showres = 1

    print("errors: ", form.errors)
    
    return render_template('form.html',
                            form=form,
                            image=path.join("static", "plot.png"), 
                            showres=showres,
                            total_power_consumed = new_outp.total_power_consumed,
                            parking_events = new_outp.parking_events,
                            charge_rate = new_entry.charging_speed,
                            efficiency = new_entry.car_efficiency,
                            count = new_entry.charging_station_count)
                            
    
if __name__ == "__main__":
    app.run()