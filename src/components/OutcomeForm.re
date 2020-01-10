open Belt;

type state = {
  label: string,
  price: string,
};

type action =
  | OnLabelChange(string)
  | OnPriceChange(string)
  | OnCancel
  | OnSubmit;

[@react.component]
let make = (~onSubmit, ~initialValue: option(Outcome.t)=?, ~onCancel) => {
  let (state, send) =
    ReactUpdate.useReducer(
      {
        label:
          initialValue->Option.map(o => o.label)->Option.getWithDefault(""),
        price:
          initialValue
          ->Option.map(o => o.price->Js.Float.toString)
          ->Option.getWithDefault(""),
      },
      (action, state) =>
      switch (action) {
      | OnLabelChange(label) => Update({...state, label})
      | OnPriceChange(price) => Update({...state, price})
      | OnCancel =>
        SideEffects(
          _ => {
            onCancel();
            None;
          },
        )
      | OnSubmit =>
        SideEffects(
          _ => {
            onSubmit(
              Outcome.{
                id:
                  initialValue
                  ->Option.map(o => o.id)
                  ->Option.getWithDefault(Js.Date.now()->Js.Float.toString),
                label: state.label,
                price: state.price->Js.Float.fromString,
              },
            );
            None;
          },
        )
      }
    );

  <form
    onSubmit={event => {
      event->ReactEvent.Form.preventDefault;
      send(OnSubmit);
    }}
    className="flex align-items-center">
    <input
      className="border px-2 py-1 rounded mr-1 focus:outline-none focus:shadow-outline"
      type_="text"
      placeholder="Label"
      onChange={event =>
        send(OnLabelChange(event->ReactEvent.Form.target##value))
      }
      value={state.label}
    />
    <input
      className="border w-24 px-2 py-1 rounded focus:outline-none focus:shadow-outline mr-2"
      type_="number"
      step=0.01
      onChange={event =>
        send(OnPriceChange(event->ReactEvent.Form.target##value))
      }
      placeholder="12"
      value={state.price}
    />
    <div className="ml-auto">
      <button
        type_="button"
        onClick={_ => send(OnCancel)}
        className="hover:bg-gray-300 border border-solid font-bold py-1 px-2 rounded text-lg mr-2">
        {j|✕|j}->React.string
      </button>
      <button
        type_="submit"
        className="bg-green-500 hover:bg-green-700 text-white font-bold py-1 px-2 rounded text-lg">
        {j|✓|j}->React.string
      </button>
    </div>
  </form>;
};