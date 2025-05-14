import React, { useState, useEffect } from 'react';
import Wallet from './components/Wallet';
import Mining from './components/Mining';
import './App.css';

function App() {
  const [balance, setBalance] = useState(0);
  const [address, setAddress] = useState('');
  const [activeTab, setActiveTab] = useState('wallet');

  useEffect(() => {
    // Load wallet data
    fetch('/api/wallet')
      .then(res => res.json())
      .then(data => {
        setBalance(data.balance);
        setAddress(data.address);
      });
  }, []);

  return (
    <div className="app">
      <header className="app-header">
        <h1>Rhodium Blockchain</h1>
        <div className="tabs">
          <button onClick={() => setActiveTab('wallet')} className={activeTab === 'wallet' ? 'active' : ''}>
            Carteira
          </button>
          <button onClick={() => setActiveTab('mining')} className={activeTab === 'mining' ? 'active' : ''}>
            Mineração
          </button>
        </div>
      </header>

      <main>
        {activeTab === 'wallet' ? (
          <Wallet balance={balance} address={address} />
        ) : (
          <Mining address={address} />
        )}
      </main>
    </div>
  );
}

export default App;