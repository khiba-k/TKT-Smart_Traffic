// Header.jsx
import React from 'react';
import { ClerkProvider, SignInButton, SignedOut, UserButton, SignedIn } from '@clerk/clerk-react';

const Header = () => {
  return (
      <header style={{display: "flex", justifyContent: "end"}}>
        <SignedOut>
          <SignInButton style={{ backgroundColor: "#288066", border: "none", color: "#FFFFFF" }} />
        </SignedOut>
        <SignedIn>
          <UserButton />
        </SignedIn>
      </header>
  );
};

export default Header;