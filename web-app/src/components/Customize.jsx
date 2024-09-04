import React from 'react'

const Customize = () => {
  return (
    <div className="card display-card" style={{ width: "200px" }}>
    <ul className="list-group list-group-flush">
      <li className="list-group-item custom-list-group2">
        <h5>Customize</h5>
      </li>
      <li className="list-group-item custom-list-group2">
        <label htmlFor="theme-toggle">Black & White Theme</label>
        <input
          type="checkbox"
          id="theme-toggle"
          onChange={(e) => handleThemeChange(e.target.checked)}
        />
      </li>
    </ul>
  </div>
  )
}

export default Customize
