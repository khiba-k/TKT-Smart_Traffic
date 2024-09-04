import React from "react";
import remove from "../assets/deleteIcon.png";

function DeleteData({ onDelete }) {
  return (
    <>  
      <button
        style={{ border: "none", borderRadius: "100%", backgroundColor: "#D9D9D9"}}
        onClick={onDelete}
      >
        <img src={remove} alt="Delete Data" />
      </button>
    </>
  );
}

export default DeleteData;
