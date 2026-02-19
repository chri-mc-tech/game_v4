#pragma once
#include <cryptopp/integer.h>
#include <iostream>

using std::string;

namespace shared::crypto {
    CryptoPP::Integer create_private_key();
    CryptoPP::Integer create_public_key(const CryptoPP::Integer& t_private_key);
    CryptoPP::Integer calculate_session_key(const CryptoPP::Integer& t_private_key, const CryptoPP::Integer& t_public_key);
    CryptoPP::SecByteBlock create_encryption_key_from_session_key(const CryptoPP::Integer& t_session_key);
    std::string encrypt_string_with_key(const string& t_string, const CryptoPP::SecByteBlock& t_encryption_key);
    std::string decrypt_string_with_key(const string& t_string, const CryptoPP::SecByteBlock& t_encryption_key);
}