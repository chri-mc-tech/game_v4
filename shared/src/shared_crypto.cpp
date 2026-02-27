#include "shared_crypto.h"
#include "shared_global.h"

#include <cryptopp/base32.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <string>

using std::string;

namespace shared::crypto {

  CryptoPP::Integer create_private_key() {
    using namespace CryptoPP;

    AutoSeededRandomPool rng;

    Integer temp_private_key(rng, Integer::One(), diffie_hellman::p - 2);
    return temp_private_key;
  }

  CryptoPP::Integer create_public_key(const CryptoPP::Integer &t_private_key) {
    using namespace CryptoPP;

    return a_exp_b_mod_c(diffie_hellman::g, t_private_key, diffie_hellman::p);
  }

  // Derives a shared session key using Diffie-Hellman
  // t_private_key: the private key of the local party (server or client)
  // t_public_key: the public key received from the other party
  CryptoPP::Integer calculate_session_key(const CryptoPP::Integer &t_private_key,
                                          const CryptoPP::Integer &t_public_key) {
    using namespace CryptoPP;

    return a_exp_b_mod_c(t_public_key, t_private_key, diffie_hellman::p);
  }

  CryptoPP::SecByteBlock create_encryption_key_from_session_key(const CryptoPP::Integer &t_session_key) {
    using namespace CryptoPP;

    string raw;
    t_session_key.Encode(StringSink(raw).Ref(), t_session_key.MinEncodedSize());

    SecByteBlock key(32);
    SHA256().CalculateDigest(key, (const CryptoPP::byte *) raw.data(), raw.size());
    return key;
  }

  // Encrypts a plaintext string using AES-CBC with the provided encryption key
  // Returns IV concatenated with ciphertext
  string encrypt_string_with_key(const string &t_string, const CryptoPP::SecByteBlock &t_encryption_key) {
    using namespace CryptoPP;

    AutoSeededRandomPool rng;

    // Generate a random IV
    CryptoPP::byte iv[AES::BLOCKSIZE];
    rng.GenerateBlock(iv, sizeof(iv));

    string cipher;

    // Set up AES-CBC encryption
    CBC_Mode<AES>::Encryption enc(t_encryption_key, t_encryption_key.size(), iv);
    StringSource(t_string, true, new StreamTransformationFilter(enc, new StringSink(cipher)));

    return string((char *) iv, AES::BLOCKSIZE) + cipher;
  }

  string decrypt_string_with_key(const string &t_string, const CryptoPP::SecByteBlock &t_encryption_key) {
    using namespace CryptoPP;

    const CryptoPP::byte *iv = (const CryptoPP::byte *) t_string.data();
    string cipher = t_string.substr(AES::BLOCKSIZE);
    string plaintext;

    CBC_Mode<AES>::Decryption dec(t_encryption_key, t_encryption_key.size(), iv);
    StringSource(cipher, true, new StreamTransformationFilter(dec, new StringSink(plaintext)));

    return plaintext;
  }

  // copy-paste from Google, sorry
  std::string generate_uuid() {
    CryptoPP::AutoSeededRandomPool r;
    CryptoPP::byte b[16];
    r.GenerateBlock(b, 16);

    std::string uuid;
    CryptoPP::Base32Encoder e(new CryptoPP::StringSink(uuid), false);
    e.Put(b, 16);
    e.MessageEnd();

    while (!uuid.empty() && uuid.back() == '=')
      uuid.pop_back();

    uuid.insert(8, "-");
    uuid.insert(17, "-");
    uuid.insert(26, "-");

    return uuid;
  }

} // namespace shared::crypto
