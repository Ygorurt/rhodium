import React, { useState } from 'react';

function Wallet({ balance, address }) {
  const [pixKey, setPixKey] = useState('');
  const [amount, setAmount] = useState('');

  const handleSendPix = () => {
    fetch('/api/send-pix', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ sender: address, pixKey, amount: parseFloat(amount) })
    })
    .then(res => res.json())
    .then(data => {
      alert(data.message);
    });
  };

  return (
    <div className="wallet">
      <h2>Sua Carteira</h2>
      <p>Endereço: {address}</p>
      <p>Saldo: {balance} RHO</p>

      <div className="send-pix">
        <h3>Enviar PIX</h3>
        <input
          type="text"
          placeholder="Chave PIX"
          value={pixKey}
          onChange={(e) => setPixKey(e.target.value)}
        />
        <input
          type="number"
          placeholder="Valor"
          value={amount}
          onChange={(e) => setAmount(e.target.value)}
        />
        <button onClick={handleSendPix}>Enviar</button>
      </div>
    </div>
  );
}

export default Wallet;