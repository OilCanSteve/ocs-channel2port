function handleEvent(event, funcs) {
  var controlToOutput = {
    ChannelA: 'ChannelAOut',
    ChannelB: 'ChannelBOut',
    ChannelC: 'ChannelCCOut',
    ChannelD: 'ChannelDOut'
  };

  var toDisplayValue = function(value) {
    var numberValue = Number(value);
    if (!Number.isFinite(numberValue)) {
      return '-';
    }

    return String(Math.round(numberValue));
  };

  var updateReadout = function(outputSymbol, value) {
    if (!event || !event.icon) {
      return;
    }

    event.icon
      .find('.mod-output-channel-readout[data-output-symbol="' + outputSymbol + '"]')
      .text(toDisplayValue(value));
  };

  if (event.type === 'start' && Array.isArray(event.ports)) {
    for (let index = 0; index < event.ports.length; index += 1) {
      const port = event.ports[index];
      const outputSymbol = controlToOutput[port.symbol];
      if (outputSymbol) {
        updateReadout(outputSymbol, port.value);
      }
    }
    return;
  }

  if (event.type === 'change' && controlToOutput[event.symbol]) {
    updateReadout(controlToOutput[event.symbol], event.value);
  }
}
